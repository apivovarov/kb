import numpy as np
import json
import sys
MAX_BATCH=1
MAX_DIM1=256
DIM2=1024
MAX_INPUT_SHAPE=(MAX_BATCH, MAX_DIM1)
# encoded text "Replace me by any text you'd like."
test_input0=np.array([[0, 9064, 6406,  162,   30,  143, 2788,   47, 1017,  101,    4,    2]], dtype=np.int32)
test_input1=np.array([[1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1]], dtype=np.int32)

with open("sizes.json") as f:
  sizes = json.load(f)

import tensorrt as trt
from cuda import cuda
from net_utils import cuda_error_check

print(f"{trt.__version__=}")
cuda_error_check(cuda.cuInit(0))
cuDevice = cuda_error_check(cuda.cuDeviceGet(0))
cuCtx = cuda_error_check(cuda.cuCtxCreate(0, cuDevice))

trt_logger = trt.Logger(trt.Logger.INFO)
runtime = trt.Runtime(trt_logger)
#fpath="roberta-large-fp32-py.trt"
#fpath="roberta-large-fp32-fds.trt" # 7.21 ms
#fpath="roberta-large-fp16-nocudagraph.trt" # 5.33
#fpath="roberta-large-fp16-fds.trt" # 7.11 ms good accuracy
#fpath="roberta-large-fp16-fds-nocudagraph.trt" # 7.0 ms good accuracy
fpath="roberta-large-fp16.trt" # 5.23 ms
fpath="roberta-large-2-fp16.trt" # 5.23 ms
fpath="roberta-large-2-fp32-fds.trt" # 7.21 ms
#fpath="roberta-large-best-nocudagraph.trt"

with open(fpath, "rb") as f:
    print("Loading:", fpath)
    engine = runtime.deserialize_cuda_engine(f.read())

assert engine is not None, "Engine is None"
context = engine.create_execution_context()
context.set_input_shape("input_ids", MAX_INPUT_SHAPE)
context.set_input_shape("attention_mask", MAX_INPUT_SHAPE)

print("Engine Info:")
for i, name in enumerate(engine):
    shape = engine.get_tensor_shape(name)
    dtype = trt.nptype(engine.get_tensor_dtype(name))
    volume = abs(trt.volume(shape))
    mode = engine.get_tensor_mode(name)
    if trt.TensorIOMode.INPUT == mode:
        desc = "input"
    elif trt.TensorIOMode.OUTPUT == mode:
        desc = "output"
    else:
        desc = "none"
    print(f"{i}  type:    {desc}\n   name:    {name} \n  dtype:    {np.dtype(dtype).name}\n  shape:    {shape} => {volume} \n")

#exit()
out_dtype = np.float32
out_dtype_bytes = 4
max_input0 = np.empty(MAX_INPUT_SHAPE, dtype=np.int32)
max_input1 = np.empty(MAX_INPUT_SHAPE, dtype=np.int32)
output0 = np.empty([MAX_BATCH, MAX_DIM1, DIM2], dtype = out_dtype)
output1 = np.empty([MAX_BATCH, DIM2], dtype = out_dtype)

# allocate device memory
d_input0 = cuda_error_check(cuda.cuMemAlloc(1 * max_input0.nbytes))
d_input1 = cuda_error_check(cuda.cuMemAlloc(1 * max_input1.nbytes))
d_output0 = cuda_error_check(cuda.cuMemAlloc(1 * output0.nbytes))
d_output1 = cuda_error_check(cuda.cuMemAlloc(1 * output1.nbytes))
bindings = [int(d_input0), int(d_input1), int(d_output0), int(d_output1)]
stream = cuda_error_check(cuda.cuStreamCreate(0))


def predict(inp, am): # result gets copied into output
    context.set_input_shape("input_ids", inp.shape)
    context.set_input_shape("attention_mask", am.shape)
    dim0 = inp.shape[0]
    dim1 = inp.shape[1]
    #print(f"{dim1=}")
    # transfer input data to device
    cuda_error_check(
        cuda.cuMemcpyHtoDAsync(d_input0, inp, inp.nbytes, stream)
    )
    cuda_error_check(
        cuda.cuMemcpyHtoDAsync(d_input1, am, am.nbytes, stream)
    )
    # execute model
    context.execute_async_v2(bindings, stream, None)
    # transfer predictions back
    cuda_error_check(
        cuda.cuMemcpyDtoHAsync(output0, d_output0, dim0*dim1*DIM2*out_dtype_bytes, stream)
    )
    cuda_error_check(
        cuda.cuMemcpyDtoHAsync(output1, d_output1, dim0*DIM2*out_dtype_bytes, stream)
    )
    # synchronize threads
    cuda_error_check(
        cuda.cuStreamSynchronize(stream)
    )

predict(test_input0, test_input1)
print("------------------------------------------")
print(output0[0,7])
print("------------------------------------------")
print(output1)
print("------------------------------------------")

print("Smoke test done")

def toptop():
  import torch
  torch.set_grad_enabled(False)
  tt = torch.from_numpy(output0[0,7])
  tts = tt.softmax(-1)
  print(tts)
  top5 = tts.topk(5)
  print(top5)
  print("Etalon: [547, 539, 616, 280, 478]")

#toptop()
#exit()


# Warmup
for i in range(100):
  sz = sizes[i%1000]
  print(i, (1,sz))
  inp = np.random.randint(0,500,size=(1,sz),dtype=np.int32)
  am = np.ones((1,sz),dtype=np.int32)
  predict(inp, am)

# Measure Latency
import time
TT=[]
N = 1000
for i in range(N):
  sz = sizes[i%1000]
  print(i, (1,sz))
  inp = np.random.randint(0,500,size=(1,sz),dtype=np.int32)
  am = np.ones((1,sz),dtype=np.int32)
  t0=time.time()
  predict(inp, am)
  t1=time.time()
  TT.append((t1-t0)*1000/MAX_INPUT_SHAPE[0])

print("AVG time (ms):",np.mean(TT))
print("P50 time (ms):",np.percentile(TT, 50))
print("P95 time (ms):",np.percentile(TT, 95))
