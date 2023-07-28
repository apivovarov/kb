import os
import pickle
import textwrap
import logging
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

import time
import numpy as np
# Warmup
with torch.inference_mode():
    N = 100
    for i in range(N):
      x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
      out = model(x)
# Test
with torch.inference_mode():
    N = 1000
    TT = []
    for i in range(N):
      x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
      t0 = time.time()
      out = model(x)
      t1 = time.time()
      TT.append(t1-t0)

avg_time = np.mean(TT) * 1000
print(f"avg time: {avg_time} ms")
