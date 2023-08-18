# ==== roberta-large Compiled ====
import numpy as np
import torch
from transformers import RobertaTokenizer, RobertaModel
import transformers
import json

print(f"{torch.__version__=}")

torch.set_grad_enabled(False)

tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
#model = RobertaModel.from_pretrained('roberta-large').cuda().eval()
model = RobertaModel.from_pretrained('roberta-large').cuda().half().eval()

with open("sizes.json") as f:
    sizes = json.load(f)
print(sizes)
#sizes = np.random.randint(3,256, size=(1000,))

def evaluate(mod, inp, am):
    return mod(inp, am)


import torch._dynamo
torch._dynamo.reset()

evaluate_opt = torch.compile(evaluate, mode="reduce-overhead", dynamic=True)
#evaluate_opt = torch.compile(evaluate, backend="tensorrt")

# Warmup
N = 100
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    sz = sizes[i]
    print(i, sz)
    inp = torch.randint(low=0,high=500,size=(1,sz), dtype=torch.int64).cuda()
    am = torch.ones((1,sz), dtype=torch.int64).cuda()
    out = evaluate_opt(model, inp, am)


N = 1000
TT = []
start = torch.cuda.Event(enable_timing=True)
end = torch.cuda.Event(enable_timing=True)
#with torch.inference_mode():
with torch.no_grad():
  for i in range(N):
    sz = sizes[i]
    print(i, sz)
    inp = torch.randint(low=0,high=500,size=(1,sz), dtype=torch.int64)
    am = torch.ones((1,sz), dtype=torch.int64)
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

print(TT[:5])
avg_time = np.mean(TT)
print(f"avg time: {avg_time} ms")

print("----------------------------------------")
with torch.no_grad():
    text = "Replace me by any text you'd like."
    encoded_input = tokenizer(text, return_tensors='pt')
    print(encoded_input)
    inp = encoded_input['input_ids'].cuda()
    am = encoded_input['attention_mask'].cuda()
    output = evaluate_opt(model, inp, am)
    logits = output['last_hidden_state'][0,7]
    logits = logits.softmax(axis=-1)
    topk = logits.topk(5)
    print("logits[0,7,:]")
    print(logits)
    print(f"{topk=}")
    print("----------------------------------------")
    print("Etalon:", [547, 539, 616, 280, 478])


