Introduction
============

.. image:: https://readthedocs.org/projects/adafruit-circuitpython-ssd1675/badge/?version=latest
    :target: https://circuitpython.readthedocs.io/projects/ssd1675/en/latest/
    :alt: Documentation Status

.. image:: https://img.shields.io/discord/327254708534116352.svg
    :target: https://adafru.it/discord
    :alt: Discord

.. image:: https://github.com/adafruit/Adafruit_CircuitPython_SSD1675/workflows/Build%20CI/badge.svg
    :target: https://github.com/adafruit/Adafruit_CircuitPython_SSD1675/actions
    :alt: Build Status

CircuitPython `displayio` drivers for SSD1675-based ePaper displays


Dependencies
=============
This driver depends on:

* `Adafruit CircuitPython <https://github.com/adafruit/circuitpython>`_

Please ensure all dependencies are available on the CircuitPython filesystem.
This is easily achieved by downloading
`the Adafruit library and driver bundle <https://github.com/adafruit/Adafruit_CircuitPython_Bundle>`_.

Installing from PyPI
=====================
.. note:: This library is not available on PyPI yet. Install documentation is included
   as a standard element. Stay tuned for PyPI availability!

On supported GNU/Linux systems like the Raspberry Pi, you can install the driver locally `from
PyPI <https://pypi.org/project/adafruit-circuitpython-ssd1675/>`_. To install for current user:

.. code-block:: shell

    pip3 install adafruit-circuitpython-ssd1675

To install system-wide (this may be required in some cases):

.. code-block:: shell

    sudo pip3 install adafruit-circuitpython-ssd1675

To install in a virtual environment in your current project:

.. code-block:: shell

    mkdir project-name && cd project-name
    python3 -m venv .env
    source .env/bin/activate
    pip3 install adafruit-circuitpython-ssd1675

Usage Example
=============

.. code-block:: python

    """Simple test script for 2.13" 250x122 black and white featherwing.

    Supported products:
      * Adafruit 2.13" Black and White FeatherWing
        * https://www.adafruit.com/product/4195
      """

    import time
    import board
    import busio
    import displayio
    import adafruit_ssd1675

    displayio.release_displays()

    epd_cs = board.D9
    epd_dc = board.D10

    display_bus = displayio.FourWire(board.SPI(), command=epd_dc, chip_select=epd_cs, baudrate=1000000)
    time.sleep(1)

    display = adafruit_ssd1675.SSD1675(display_bus, width=250, height=122, rotation=90)

    g = displayio.Group()

    f = open("/display-ruler.bmp", "rb")

    pic = displayio.OnDiskBitmap(f)
    t = displayio.TileGrid(pic, pixel_shader=displayio.ColorConverter())
    g.append(t)

    display.show(g)

    display.refresh()

    print("refreshed")

    time.sleep(120)

Contributing
============

Contributions are welcome! Please read our `Code of Conduct
<https://github.com/adafruit/Adafruit_CircuitPython_SSD1675/blob/master/CODE_OF_CONDUCT.md>`_
before contributing to help this project stay welcoming.

Documentation
=============

For information on building library documentation, please check out `this guide <https://learn.adafruit.com/creating-and-sharing-a-circuitpython-library/sharing-our-docs-on-readthedocs#sphinx-5-1>`_.
