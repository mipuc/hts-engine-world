# hts-engine-world

This software contains an integration of the WORLD vocoder (https://github.com/mmorise/World) and hts_engine API-1.10 (http://hts-engine.sourceforge.net). It also uses functions from the Speech Signal Processing Toolkit (SPTK) (http://sp-tk.sourceforge.net/).

The main modification can be found in [lib/synthworld.cpp](lib/synthworld.cpp).

Use

./configure

and

make

to create bin/hts_engine.

You have to call hts_engine with the option -w to use the WORLD vocoder, otherwise the default vocoder will be used.

*@misc{pucher2018,*<br/>
*author = {Pucher, Michael},*<br/>
*title = {Hts-engine-world: An integration of the WORLD vocoder and hts_engine API-1.10},*<br/>
*year = {2018},*<br/>
*publisher = {GitHub},*<br/>
*journal = {GitHub repository},*<br/>
*howpublished = {\\url{https://github.com/mipuc/hts-engine-world}},*<br/>
*}*

Michael Pucher michael.pucher@oeaw.ac.at
