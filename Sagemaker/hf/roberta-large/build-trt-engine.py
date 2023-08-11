import logging
import os
import tensorrt as trt
import builtins

#from net_utils import EngineCalibrator

logging.basicConfig(level=logging.DEBUG)
logging.getLogger("EngineBuilder").setLevel(logging.DEBUG)
log = logging.getLogger("EngineBuilder")

onnx_path = "roberta-large.onnx"
engine_path = "./roberta-large-fp32-py.trt"

trt_logger = trt.Logger(trt.Logger.INFO)

trt.init_libnvinfer_plugins(trt_logger, namespace="")

builder = trt.Builder(trt_logger)
config = builder.create_builder_config()
config.profiling_verbosity=trt.ProfilingVerbosity.DETAILED
#config.flags=0
#config.set_flag(trt.BuilderFlag.FP16)
#config.set_flag(trt.BuilderFlag.INT8)
#config.int8_calibrator = EngineCalibrator(cache_file)
#config.set_flag(trt.BuilderFlag.OBEY_PRECISION_CONSTRAINTS)
#config.set_flag(trt.BuilderFlag.SPARSE_WEIGHTS)
config.set_preview_feature(trt.PreviewFeature.FASTER_DYNAMIC_SHAPES_0805, True)


max_ws_size = 15 * (2 ** 30)
print("================== max_ws_size", type(max_ws_size), max_ws_size)
config.set_memory_pool_limit(trt.MemoryPoolType.WORKSPACE, max_ws_size)

network_flags = (1 << int(trt.NetworkDefinitionCreationFlag.EXPLICIT_BATCH))
network = builder.create_network(network_flags)
parser = trt.OnnxParser(network, trt_logger)

onnx_path = os.path.realpath(onnx_path)

with open(onnx_path, "rb") as f:
    if not parser.parse(f.read(), onnx_path):
        log.error("Failed to load ONNX file: {}".format(onnx_path))
        for error in range(parser.num_errors):
            log.error(parser.get_error(error))
        exit(1)

print(f"=========================== {onnx_path} Loaded")


def desc_layer(l, i):
    print(i, l.name, l.type, l.num_inputs, l.num_outputs, l.precision, l.precision_is_set)
    for j in range(l.num_outputs):
        t = l.get_output(j)
        print("    output", j, t.name, t.shape, l.get_output_type(j), l.output_type_is_set(j))


num_layers = network.num_layers
print("num_layers:", num_layers)
for i in range(num_layers):
    l = network.get_layer(i)
    #desc_layer(l, i)
    # l.precision = trt.float32
    # for j in range(l.num_outputs):
    #     l.set_output_type(j, trt.float32)
    # if "/model/transformer/h.0/attn/attention/Softmax" in l.name:
    #     l.precision = trt.float32
    #     l.set_output_type(0, trt.float32)
    #     l_prev = network.get_layer(i-1)
    #     l_prev.precision = trt.float32
    #     l_prev.set_output_type(0, trt.float32)
    #     l_next = network.get_layer(i + 1)
    #     l_next.precision = trt.float32
    #     l_next.set_output_type(0, trt.float32)
    #     desc_layer(l, i)

#exit()
inputs = [network.get_input(i) for i in range(network.num_inputs)]
dynamic_inputs = False
for a in inputs:
    log.info("Input '{}' with shape {} and dtype {}".format(a.name, a.shape, a.dtype))

outputs = [network.get_output(i) for i in range(network.num_outputs)]
for output in outputs:
    log.info("Output '{}' with shape {} and dtype {}".format(output.name, output.shape, output.dtype))

#exit()
profile = builder.create_optimization_profile()
for a in inputs:
    min_shape = [1,1]
    opt_shape = [1,12]
    max_shape = [8,256]
    profile.set_shape(a.name, min_shape, opt_shape, max_shape)
    log.info("Input '{}' Optimization Profile with shape MIN {} / OPT {} / MAX {}".format(
        a.name, min_shape, opt_shape, max_shape))

config.add_optimization_profile(profile)

engine_path = os.path.realpath(engine_path)
log.info("Building Engine in {}".format(engine_path))
log.info(f"builder.platform_has_tf32: {builder.platform_has_tf32}")
log.info(f"builder.platform_has_fast_fp16: {builder.platform_has_fast_fp16}")
log.info(f"builder.platform_has_fast_int8: {builder.platform_has_fast_int8}")
log.info(f"max_threads: {builder.max_threads}")
log.info(f"num_DLA_cores: {builder.num_DLA_cores}")
log.info(f"max_DLA_batch_size: {builder.max_DLA_batch_size}")

log.info(f"MemoryPoolType.WORKSPACE: {config.get_memory_pool_limit(trt.MemoryPoolType.WORKSPACE)}")
log.info(f"MemoryPoolType.DLA_MANAGED_SRAM: {config.get_memory_pool_limit(trt.MemoryPoolType.DLA_MANAGED_SRAM)}")
log.info(f"MemoryPoolType.DLA_LOCAL_DRAM: {config.get_memory_pool_limit(trt.MemoryPoolType.DLA_LOCAL_DRAM)}")
log.info(f"MemoryPoolType.DLA_GLOBAL_DRAM: {config.get_memory_pool_limit(trt.MemoryPoolType.DLA_GLOBAL_DRAM)}")

log.info(f"BuilderFlag.TF32: {config.get_flag(trt.BuilderFlag.TF32)}")
log.info(f"BuilderFlag.FP16: {config.get_flag(trt.BuilderFlag.FP16)}")
log.info(f"BuilderFlag.INT8: {config.get_flag(trt.BuilderFlag.INT8)}")
log.info(f"BuilderFlag.OBEY_PRECISION_CONSTRAINTS: {config.get_flag(trt.BuilderFlag.OBEY_PRECISION_CONSTRAINTS)}")
log.info(f"PreviewFeature.FASTER_DYNAMIC_SHAPES_0805: {config.get_preview_feature(trt.PreviewFeature.FASTER_DYNAMIC_SHAPES_0805)}")
log.info(f"BuilderFlag.SPARSE_WEIGHTS: {config.get_flag(trt.BuilderFlag.SPARSE_WEIGHTS)}")

#config.avg_timing_iterations=8
log.info(f"avg_timing_iterations: {config.avg_timing_iterations}")

trt_logger.min_severity = trt.Logger.Severity.VERBOSE

builtins.input("Press any key to continue")

log.info("==== builder.build_serialized_network")
engine_bytes = builder.build_serialized_network(network, config)
if engine_bytes:
    with open(engine_path, "wb") as f:
        log.info("==== Serializing engine to file: {:}".format(engine_path))
        f.write(engine_bytes)
else:
    print("Failed to create engine_bytes")
