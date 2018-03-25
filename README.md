brownout - simplistic screen color shifter
==========================================
brownout is intended to adjust the color temperature of the screen. It is a 
direct adaptation of Ted Unangst blog post [0], which also explains the 
motivation behind it.

The basic xrandr tool is not able to achieve the desired result and alternative
programs are bloated compared to the given solution. The alternatives are
either [flux][1] or [redshift][2]. The former is encumbered by only being
provided as a blob. The later is overall more complex than brownout.

The term "temperature" is used only loosely, since the colors will be adjusted
to the users preference, not to any fixed temperature scale.

[0]: http://www.tedunangst.com/flak/post/sct-set-color-temperature
[1]: https://justgetflux.com/
[2]: https://github.com/jonls/redshift

Running brownout
----------------

Adjusting the color of attached displays to 42 percent of the curve:

    ./brownout 42

Setting the screen colors back to normal:

    ./brownout 0

Decreasing the current level by 5 percent:

    ./brownout -5

Increasing the current level by 13 percent:

    ./brownout +13


Configuration
-------------
The configuration of brownout is done by creating a custom config.h
and (re)compiling the source code.

For automatic adjustments based on estimates of available sunlight, you can use
use the provided tool 'is-day'. It takes two parameters, indicating the
latitude and longitude in degrees, respectively.

For Hamburg, Germany:

    ./is-day 53.5 -10

It prints one of 'day', 'sunset', 'dusk', 'night', 'dawn', 'sunrise', giving
an indication of how fast the lighting is changing currently. The return code
will also indicate, if it is night or day. You could setup a cronjob:

    */1 * * * * * sh -c 'is-day 53.3 -10 && brownout -1 || brownout +1'

Or write a shell script. See the provided "brownoutd" file for an example.


Requirements
------------
In order to build brownout you need the Xlib header files.


Installation
------------
Edit config.mk to match your local setup (brownout is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install brownout (if
necessary as root):

    make clean install

