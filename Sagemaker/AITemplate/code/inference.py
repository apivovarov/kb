import logging
from diffusers import EulerDiscreteScheduler
import torch
import base64
import json
import numpy as np
from pipeline_stable_diffusion_ait import StableDiffusionAITPipeline

def process_data(data: dict) -> dict:
    assert "prompt" in data, 'Key "prompt" not found in the input json'
    prompt_arr = data["prompt"] # SD pipeline supports str or List[str]
    return {
        "prompt": prompt_arr,
        "guidance_scale": data.get("guidance_scale", 7.5),
        "num_inference_steps": min(data.get("num_inference_steps", 50), 50),
        "height": 512,
        "width": 512,
    }

def model_fn(model_dir: str, context):
    gpu_id = context.system_properties.get("gpu_id")
    logging.info(f"Loading model from {model_dir=}, {gpu_id=}")
    pipe = StableDiffusionAITPipeline.from_pretrained(
        model_dir,
        scheduler=EulerDiscreteScheduler.from_pretrained(
            model_dir, subfolder="scheduler"
        ),
        revision="fp16",
        torch_dtype=torch.float16,
    )
    if torch.cuda.is_available():
        pipe = pipe.to(f"cuda:{gpu_id}")

    logging.info(f"Model was loaded from {model_dir=} to {gpu_id=}")
    return pipe

def input_fn(input_data, content_type):
    logging.debug(f"Run input_fn. {input_data=}, {content_type=}")
    assert content_type == 'application/json', "Unexpected {content_type=}. Need application/json"
    data = json.loads(input_data)
    return data

def predict_fn(data: dict, pipe) -> dict:
    with torch.autocast("cuda"):
        images = pipe(**process_data(data))["images"]

    # return dictionary, which will be json serializable
    return {
        "images": [
            base64.b64encode(np.array(image).astype(np.uint8)).decode("utf-8")
            for image in images
        ]
    }
