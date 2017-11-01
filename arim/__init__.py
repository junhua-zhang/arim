"""
A Python library for array imaging and modelling in ultrasonic testing
"""

from . import settings
from . import exceptions
from .core import *

from . import _probes, geometry, registration, config, helpers, ut
from .geometry import Points, Grid


# probe database
probes = _probes.probes

#
L = Mode.L
T = Mode.T


__author__ = "Nicolas Budyn"
__credits__ = []
__license__ = "All rights reserved"
__copyright__ = "2016, 2017, Nicolas Budyn"

# Must respect PEP 440: https://www.python.org/dev/peps/pep-0440/
# Must be bumped at each release
__version__ = '0.8.dev0'