# ==== roberta-large Compiled ====
import numpy as np
import torch
from transformers import RobertaTokenizer, RobertaModel

torch.set_grad_enabled(False)

tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
#model = RobertaModel.from_pretrained('roberta-large').cuda().eval()
model = RobertaModel.from_pretrained('roberta-large').cuda().half().eval()

def evaluate(mod, inp):
    #with torch.inference_mode():
    with torch.no_grad():
        return mod(inp)


import torch._dynamo
torch._dynamo.reset()

# reduce-overhead mode is important to get speedup in fp16 precision
evaluate_opt = torch.compile(evaluate, mode="reduce-overhead")

# Warmup
N = 100
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
    out = evaluate_opt(model, x)

N = 1000
TT = []
start = torch.cuda.Event(enable_timing=True)
end = torch.cuda.Event(enable_timing=True)
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
    start.record()
    out = evaluate_opt(model, x)
    end.record()
    torch.cuda.synchronize()
    TT.append(start.elapsed_time(end))

avg_time = np.mean(TT)
print(f"avg time: {avg_time} ms")
