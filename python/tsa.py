""" tsa.py

This is a helper module for writing Python applications that interact
with a Medoc TSA-II via the accompanying hardware TSA-to-USB Adapter.


BASIC USAGE:

>>> import tsa
>>> tsa.connect()
>>> tsa.trigger()  # sends a pulse to the TSA
>>> tsa.wait()     # this call will block until a pulse is received
True
>>> tsa.check()    # has a pulse been received since last check?
False
>>> tsa.check()
False
>>> tsa.check()
True
>>> tsa.check()
False
>>> tsa.disconnect()


DEPENDENCIES:

Install pyserial by running from the commandline:
$ pip install -r requirements.txt


COPYRIGHT:

Written by Devon Rueckner for Carissa Cascio
Vanderbilt University Medical Center
2015

"""


#########################################################################
## Dependencies

import serial
import serial.tools.list_ports


#########################################################################
## API


def trigger():
    """
    Sends a TTL pulse to the TSA

    Arguments: N/A
    Returns: N/A
    """
    _tsa.checkPort()
    _tsa.port.write(_TRIGGER_CHAR)


def wait(timeout=None):
    """
    Blocks until either a TTL pulse is received from the TSA, or
    until `timeout` seconds have elapsed.

    Arguments: `timeout` (Optional, seconds. Default is to wait indefinitely.)
    Returns: True if a pulse was received, False if the wait timed out
    """
    _tsa.port.flushInput()
    _tsa.port.timeout = timeout
    result = _tsa.port.read()
    if result == _TRIGGER_CHAR:
        return True
    elif result == '':
        return False
    else:
        raise TSAAdapterException("Unnexpected result: '{}'".format(result))


def check():
    """
    Checks whether a pulse was received. If called frequently, this function can
    be used to check (poll) the TSA for recent pulse events.

    Arguments: N/A
    Returns: True if a pulse was received, False otherwise

    Be aware that timing errors can result from slow polling periods: for example, if
    this function is called only once per second, the pulse may have arrived any
    time in the one-second period. For more accurate timing, use `wait`, potentially
    running it in another thread.

    Also, note that if multiple pulses are received between calls to `check`, they
    check will still simply return True once, so that extra information would be lost.
    """
    _tsa.checkPort()
    if _tsa.port.inWaiting():
        result = _tsa.port.read()
        _tsa.port.flushInput()
        if result == _TRIGGER_CHAR:
            return True
        else:
            raise TSAAdapterException("Unnexpected result: '{}'".format(result))
    return False


def connect():
    """
    Connects the application to the TSA-II adapter.
    Raises a TSAAdapterException if it could not connect.

    Searches for a serial port that contains the string 'usb'.
    This will commonly be `/dev/tty.usbmodem411`.

    Arguments: N/A
    Returns: N/A
    """
    ports = [p[0] for p in serial.tools.list_ports.grep("usb")]
    if len(ports):
        try:
            _tsa.port = serial.Serial(ports[0], 9600)
        except serial.serialutil.SerialException, e:
            raise TSAAdapterException("Could not connect to '{0}'. You may need to run with `sudo`".format(e))
    else:
        raise TSAAdapterException("Adapter not found. Is it plugged in?")
    _tsa.port.write(_CONNECT_CHAR)


def disconnect():
    """
    Disconnects the application from the TSA-II adapter.

    Arguments: N/A
    Returns: N/A
    """
    _tsa.checkPort()
    _tsa.port.close()
    _tsa.port = None


class TSAAdapterException(Exception):
    pass


#########################################################################
## Module Internals

_TRIGGER_CHAR = '#'
_CONNECT_CHAR = '!'

class _TSA:
    """
    serial port state
    """
    def __init__(self):
        self.port = None

    def checkPort(self):
        """
        Ensure that the serial port exists and is open.
        """
        if self.port is None:
            raise TSAAdapterException('No connection made. You need to call `connect` first.')
        if not self.port.isOpen():
            raise TSAAdapterException('Connection to adapter was lost.')

_tsa = _TSA()
