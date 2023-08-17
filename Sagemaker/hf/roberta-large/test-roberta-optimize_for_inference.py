# dynamo + jit.script + optimize_for_inference
from typing import List
import numpy as np
import torch
from transformers import RobertaTokenizer, RobertaModel
import transformers


torch.set_grad_enabled(False)

def optimize_for_inference_compiler(gm: torch.fx.GraphModule, example_inputs: List[torch.Tensor]):
    #traced =  torch.jit.trace(gm, (example_inputs[0],example_inputs[1]))
    #return traced
    scripted = torch.jit.script(gm, example_inputs=example_inputs)
    return torch.jit.optimize_for_inference(scripted)
    #return gm.forward


tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
# fp32
model = RobertaModel.from_pretrained('roberta-large').cuda().eval()
# fp16
#model = RobertaModel.from_pretrained('roberta-large').cuda().half().eval()

def evaluate(mod, inp, am):
    return mod(inp, am)


import torch._dynamo
torch._dynamo.reset()

#evaluate_opt = torch.compile(evaluate, mode="reduce-overhead")
#evaluate_opt = torch.compile(evaluate, backend="tensorrt")
evaluate_opt = torch.compile(evaluate, backend=optimize_for_inference_compiler)

# Warmup
N = 100
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
    am = torch.ones((1,12)).cuda()
    out = evaluate_opt(model, x, am)

#exit()

N = 1000
TT = []
start = torch.cuda.Event(enable_timing=True)
end = torch.cuda.Event(enable_timing=True)
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64)
    am = torch.ones((1,12)).cuda()
    start.record()
    x = x.cuda()
    out = evaluate_opt(model, x, am)
    #last_hidden_state = out['last_hidden_state']
    #last_hidden_state = last_hidden_state.cpu()
    #pooler_output = out['pooler_output']
    #pooler_output = pooler_output.cpu()
    end.record()
    torch.cuda.synchronize()
    TT.append(start.elapsed_time(end))

avg_time = np.mean(TT)
print(f"avg time: {avg_time} ms")
