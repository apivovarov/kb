# ==== roberta-large Compiled ====
import numpy as np
import torch
from transformers import RobertaTokenizer, RobertaModel
import transformers

print(f"{torch.__version__=}")

torch.set_grad_enabled(False)

tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
#model = RobertaModel.from_pretrained('roberta-large').cuda().eval()
model = RobertaModel.from_pretrained('roberta-large').cuda().half().eval()

def evaluate(mod, inp, am):
    return mod(inp, am)


import torch._dynamo
torch._dynamo.reset()

evaluate_opt = torch.compile(evaluate, mode="reduce-overhead")
#evaluate_opt = torch.compile(evaluate, backend="tensorrt")

# Warmup
N = 100
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    inp = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
    am = torch.ones((1,12)).cuda()
    out = evaluate_opt(model, inp, am)


N = 1000
TT = []
start = torch.cuda.Event(enable_timing=True)
end = torch.cuda.Event(enable_timing=True)
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    inp = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64)
    am = torch.ones((1,12))
    start.record()
    inp = inp.cuda()
    am = am.cuda()
    out = evaluate_opt(model, inp, am)
    last_hidden_state = out['last_hidden_state']
    #last_hidden_state = last_hidden_state.cpu()
    pooler_output = out['pooler_output']
    #pooler_output = pooler_output.cpu()
    #print(pooler_output[0,10])
    #print(last_hidden_state[0,2,10])
    end.record()
    torch.cuda.synchronize()
    TT.append(start.elapsed_time(end))

print(TT[:5]) # check if first duration is low (means no recompilation)
avg_time = np.mean(TT)
print(f"avg time: {avg_time} ms")
