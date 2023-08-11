import numpy as np
import sys
INPUT_SHAPE=(1,12)
# encoded text "Replace me by any text you'd like."
test_input=np.array([[0, 9064, 6406,  162,   30,  143, 2788,   47, 1017,  101,    4,    2]], dtype=np.int32)
batch = np.random.randint(0,500,size=INPUT_SHAPE,dtype=np.int32)

import tensorrt as trt
from cuda import cuda
from net_utils import cuda_error_check

cuda_error_check(cuda.cuInit(0))
cuDevice = cuda_error_check(cuda.cuDeviceGet(0))
cuCtx = cuda_error_check(cuda.cuCtxCreate(0, cuDevice))

trt_logger = trt.Logger(trt.Logger.INFO)
runtime = trt.Runtime(trt_logger)
fpath="roberta-large-fp32-py.trt"
fpath="roberta-large-fp32-fds.trt" # 7.21 ms
#fpath="roberta-large-fp16-nocudagraph.trt" # 5.33
fpath="roberta-large-fp16-fds.trt" # 7.11 ms good accuracy
fpath="roberta-large-fp16-fds-nocudagraph.trt" # 7.0 ms good accuracy
#fpath="roberta-large-fp16-cudagraph.trt" # 5.23 ms
#fpath="roberta-large-best-nocudagraph.trt"

with open(fpath, "rb") as f:
    print("Loading:", fpath)
    engine = runtime.deserialize_cuda_engine(f.read())

assert engine is not None, "Engine is None"
context = engine.create_execution_context()
context.set_input_shape("input_ids", INPUT_SHAPE)

print("Engine Info:")
for i, binding in enumerate(engine):
    shape = [engine.max_batch_size, *engine.get_binding_shape(binding)]
    dtype = trt.nptype(engine.get_binding_dtype(binding))
    volume = abs(trt.volume(engine.get_binding_shape(binding)))
    if engine.binding_is_input(binding):
        desc = "input"
    else:
        desc = "output"
    print(f"{i} type:    {desc}\n  binding: {binding} \n  data:    {np.dtype(dtype).name}\n  shape:   {shape} => {volume} \n")

#exit()
target_dtype = np.float32

output0 = np.empty([1, 12, 1024], dtype = target_dtype)
output1 = np.empty([1, 1024], dtype = target_dtype)

# allocate device memory
d_input = cuda_error_check(cuda.cuMemAlloc(1 * batch.nbytes))
d_output0 = cuda_error_check(cuda.cuMemAlloc(1 * output0.nbytes))
d_output1 = cuda_error_check(cuda.cuMemAlloc(1 * output1.nbytes))
bindings = [int(d_input), int(d_output0), int(d_output1)]
stream = cuda_error_check(cuda.cuStreamCreate(0))


def predict(batch): # result gets copied into output
    # transfer input data to device
    cuda_error_check(
        cuda.cuMemcpyHtoDAsync(d_input, batch, batch.nbytes, stream)
    )
    # execute model
    context.execute_async_v2(bindings, stream, None)
    # transfer predictions back
    cuda_error_check(
        cuda.cuMemcpyDtoHAsync(output0, d_output0, output0.nbytes, stream)
    )
    cuda_error_check(
        cuda.cuMemcpyDtoHAsync(output1, d_output1, output1.nbytes, stream)
    )
    # synchronize threads
    cuda_error_check(
        cuda.cuStreamSynchronize(stream)
    )

predict(test_input)
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

#toptop()

#exit()
# Warmup
for i in range(100):
  batch = np.random.randint(0,500,size=INPUT_SHAPE,dtype=np.int32)
  predict(batch)

# Measure Latency
import time
TT=[]
N = 3000
for i in range(N):
  batch = np.random.randint(0,500,size=INPUT_SHAPE,dtype=np.int32)
  t0=time.time()
  predict(batch)
  t1=time.time()
  TT.append((t1-t0)*1000/INPUT_SHAPE[0])

print("AVG time (ms):",np.mean(TT))
print("P50 time (ms):",np.percentile(TT, 50))
print("P95 time (ms):",np.percentile(TT, 95))
