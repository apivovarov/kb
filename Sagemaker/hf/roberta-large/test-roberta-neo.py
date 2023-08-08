import numpy as np
import os
import pickle
import torch
import neopytorch
from sagemaker_inference import content_types, decoder, default_inference_handler, encoder

torch.set_grad_enabled(False)

# Uncomment to enable FP16
#os.environ['TVM_TENSORRT_USE_FP16'] = '1'

model_path="/opt/ml/model/roberta-large.pt"
model_dir="/opt/ml/model"
device = torch.device("cuda") # if torch.cuda.is_available() else "cpu")

neopytorch.config(model_dir=model_dir, neo_runtime=True)
model = torch.jit.load(model_path, map_location=device)
model = model.to(device)

with open(os.path.join(model_dir, 'sample_input.pkl'), 'rb') as input_file:
  model_input = pickle.load(input_file)

model_input = model_input[0]
model_input = model_input.to(device)
print(f"{model_input.dtype=}, {model_input.shape=}")
out = model(model_input)

# Warmup
N = 100
with torch.inference_mode():
    for i in range(N):
      x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
      out = model(x)
# Test
N = 1000
TT = []
start = torch.cuda.Event(enable_timing=True)
end = torch.cuda.Event(enable_timing=True)
with torch.inference_mode():
    for i in range(N):
      x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
      start.record()
      out = model(x)
      end.record()
      torch.cuda.synchronize()
      TT.append(start.elapsed_time(end))

avg_time = np.mean(TT)
print(f"avg time: {avg_time} ms")
