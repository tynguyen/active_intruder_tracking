"""
@Author: Ty Nguyen
@Email: tynguyen.tech@gmail.com
@Brief: quadric equation optimization using pytorch
"""
from typing import Dict, List, Tuple, Union
import torch
from torch.autograd import Variable
import torch.nn as nn
from torch.optim import SGD,  Adam, RMSprop
import numpy as np
import torch.nn.functional as F
import matplotlib.pyplot as plt

def data_generator(data_size:int = 50, seed:int = 0) -> Tuple[List[float], List[float]]:
    """
    Generate data for quadric equation optimization
    :param data_size: number of data points
    :param seed: random seed
    :return: x, y
        fx = y = 8x^2 + 4x - 3
    """
    np.random.seed(seed=seed)
    x = np.random.rand(data_size).reshape(-1, 1)
    y = np.array([8 * x_i ** 2 + 4 * x_i - 3 for x_i in x])
