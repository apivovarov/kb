import os
import logging

import numpy as np
import tensorrt as trt
from cuda import cuda

log = logging.getLogger("EngineBuilder")

def cuda_error_check(args):
    """CUDA error checking."""
    err, ret = args[0], args[1:]
    if isinstance(err, cuda.CUresult):
      if err != cuda.CUresult.CUDA_SUCCESS:
        raise RuntimeError("Cuda Error: {}".format(err))
    else:
      raise RuntimeError("Unknown error type: {}".format(err))
    # Special case so that no unpacking is needed at call-site.
    if len(ret) == 1:
      return ret[0]
    return ret


class EngineCalibrator(trt.IInt8MinMaxCalibrator):
    """
    Implements the INT8 MinMax Calibrator.
    """

    def __init__(self, cache_file):
      """
      :param cache_file: The location of the cache file.
      """
      super().__init__()
      self.cache_file = cache_file

      from torchvision.io import read_image
      from torchvision.models import ResNet50_Weights

      img = read_image("cat.jpg")
      weights = ResNet50_Weights.DEFAULT
      preprocess = weights.transforms()
      self.batch = preprocess(img).unsqueeze(0).numpy()
      self.cnt = 0
      cuda_error_check(cuda.cuInit(0))
      cuDevice = cuda_error_check(cuda.cuDeviceGet(0))
      cuCtx = cuda_error_check(cuda.cuCtxCreate(0, cuDevice))
      self.d_batch = cuda_error_check(cuda.cuMemAlloc(1 * self.batch.nbytes))

    def get_batch_size(self):
      """
      Overrides from trt.IInt8MinMaxCalibrator.
      Get the batch size to use for calibration.
      :return: Batch size.
      """
      return 1

    def get_batch(self, names):
      """
      Overrides from trt.IInt8MinMaxCalibrator.
      Get the next batch to use for calibration, as a list of device memory pointers.
      :param names: The names of the inputs, if useful to define the order of inputs.
      :return: A list of int-casted memory pointers.
      """
      if self.cnt > 20:
        return None

      try:
        log.info(f"Calibrating data for inputs {names}")
        cuda_error_check(
          cuda.cuMemcpyHtoD(
            self.d_batch,
            np.ascontiguousarray(self.batch),
            1 * self.batch.nbytes))

        self.cnt += 1
        return [int(self.d_batch),]
      except StopIteration:
        log.info("Finished calibration batches")
        return None

    def read_calibration_cache(self):
      """
      Overrides from trt.IInt8MinMaxCalibrator.
      Read the calibration cache file stored on disk, if it exists.
      :return: The contents of the cache file, if any.
      """
      if os.path.exists(self.cache_file):
        with open(self.cache_file, "rb") as f:
          log.info("Using calibration cache file: {}".format(self.cache_file))
          return f.read()

    def write_calibration_cache(self, cache):
      """
      Overrides from trt.IInt8MinMaxCalibrator.
      Store the calibration cache to a file on disk.
      :param cache: The contents of the calibration cache to store.
      """
      if self.cache_file is None:
        return
      with open(self.cache_file, "wb") as f:
        log.info("Writing calibration cache data to: {}".format(self.cache_file))
        f.write(cache)

