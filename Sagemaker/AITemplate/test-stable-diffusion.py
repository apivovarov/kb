# Test code for huggingface stabilityai/stable-diffusion-2-1-base

import torch
from diffusers import StableDiffusionPipeline, DPMSolverMultistepScheduler
import time
import numpy as np

model_id = "stabilityai/stable-diffusion-2-1-base"

# Use the DPMSolverMultistepScheduler (DPM-Solver++) scheduler here instead
pipe = StableDiffusionPipeline.from_pretrained(
        model_id,
        revision="fp16",
        torch_dtype=torch.float16
)
pipe.scheduler = DPMSolverMultistepScheduler.from_config(pipe.scheduler.config)
pipe = pipe.to("cuda")

prompt = "a photo of an astronaut riding a horse on mars"
# Warmup
image = pipe(prompt).images[0]
TT=[]
# Test loop
for i in range(10):
  t0=time.time()
  image = pipe(prompt).images[0]
  t1=time.time()
  TT.append(t1-t0)
  print("time:", t1-t0)

print("AVG time:", np.mean(TT))
image.save("image.png")
