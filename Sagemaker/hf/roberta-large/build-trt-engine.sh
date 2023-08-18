# --fp16 \
# --preview="+fasterDynamicShapes0805" \

#fasterDynamicShapes0805 improves fp32 performance
#fasterDynamicShapes0805 slowes down fp16 performance but gives better accuracy

trtexec \
--preview="+fasterDynamicShapes0805" \
--verbose \
--useCudaGraph \
--onnx=roberta-large-2.onnx \
--saveEngine=roberta-large-2-fp32-fds.trt \
--minShapes=input_ids:1x3,attention_mask:1x3 \
--optShapes=input_ids:1x12,attention_mask:1x12 \
--maxShapes=input_ids:8x256,attention_mask:8x256


# trtexec \
# --useCudaGraph \
# --loadEngine=roberta-large-fp16-cudagraph.trt \
# --shapes=input_ids:1x12 \
# --noDataTransfers \
# --iterations=100 \
# --avgRuns=100
