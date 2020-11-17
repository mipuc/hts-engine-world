# hts-engine-world

This software contains an integration of the WORLD vocoder (https://github.com/mmorise/World) and hts_engine API-1.10 (http://hts-engine.sourceforge.net). It also uses functions from the Speech Signal Processing Toolkit (SPTK) (http://sp-tk.sourceforge.net/).

The main modification can be found in [lib/synthworld.cpp](lib/synthworld.cpp).

## Installation

Follow the classic sequence:

```sh
autoreconf --install
./configure
make
```

at the end, the executable `bin/hts_engine` will be created

## How to use

You have to call hts_engine with the option -w to use the WORLD vocoder, otherwise the default MLSA vocoder will be used.

## Reference

```bibtex
@Misc{pucher2018,
  author       = "Michael Pucher",
  title        = "Hts-engine-world: An integration of the WORLD vocoder and hts_engine API-1.10",
  year         = "2018",
  publisher    = "GitHub",
  journal      = "GitHub repository",
  howpublished = "\url{https://github.com/mipuc/hts-engine-world}",
}
```

Michael Pucher michael.pucher@oeaw.ac.at
