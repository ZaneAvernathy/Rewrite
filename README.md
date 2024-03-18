
# About

Sometimes I see code written for FE8 and think about how I'd implement it. These hacks are the result of me rewriting others' code.

These are built using both Nintenlord's EA Core and CrazyColorz5's ColorzCore to ensure compatibility.

See `CREDITS.md` for a list of hacks and authors.

### Building from source

You'll need some source of `Make` and other GNU tools (such as through `devkitARM`, which is also needed). Acquire

* [**Event Assembler**](https://feuniverse.us/t/1749)
* [**ColorzCore**](https://feuniverse.us/t/3970)
* [**CLib**](https://github.com/StanHash/FE-CLib)
* [**lyn**](https://github.com/StanHash/lyn/releases)
* [**ea-dep**](https://github.com/StanHash/ea-dep/releases)
* [**devkitARM**](https://devkitpro.org/wiki/Getting_Started)
* [**thumblib3**](https://github.com/ZaneAvernathy/thumblib3)
* and an FE8U.gba ROM image

and set up your folders like
```
<This folder>/
Tools/
  CLib/
  EventAssembler/
    Tools/
      lyn
      ea-dep
    Core
    ColorzCore
  thumblib3/
FE8U.gba
```

(Note the lack of space in `EventAssembler`)

Alternatively, see the `Makefile` and edit it how you'd like.

You can then build using `make` in the repo folder. Target options are:
* `make all`: check that all hacks build using both cores
* `make nl`: build using Nintenlord's `Core`
* `make cc`: build using CrazyColorz5's `ColorzCore`
* `make debug`: build using both but with debugging messages
* `make nl debug`: build using Nintenlord's `Core` but with debugging messages
* `make cc debug`: build using CrazyColorz5's `ColorzCore` but with debugging messages

### Using hacks individually

Each hack should have some `Installer.event` in their folder that you can include in a larger buildfile. Each installer may include files from the `SRC` directory, so check to see if you need to copy/relocate anything.

If you want to build these from source anywhere else, similar rules apply: they may include files from the `SRC` directory and/or `CLib`, so check the sources.

### Shoutouts

Some inspirations:

* **Nat/Stan**: [github](https://github.com/StanHash) | [FEU thread](https://feuniverse.us/t/2376)
* **Huichelaar**: [github](https://github.com/Huichelaar) | [FEU thread](https://feuniverse.us/t/8229)
* **Contro**: [github](https://github.com/masterofcontroversy/) | [FEU thread](https://feuniverse.us/t/13514)
* **Vesly**: [github](https://github.com/Veslyquix) | [FEU thread](https://feuniverse.us/t/12011)
* **Circles**: [github](https://github.com/boviex) | [FEU thread](https://feuniverse.us/t/13510)
* **Leonarth**: [github](https://github.com/LeonarthCG) | [FEU thread](https://feuniverse.us/t/2693)
* **Tequila**: [FEU thread](https://feuniverse.us/t/1655)
* **Sme**: [github](https://github.com/sme23) | [FEU Thread](https://feuniverse.us/t/4445)
* **Snakey**: [github](https://github.com/Snakey11) | [FEU Thread](https://feuniverse.us/t/3627)

There are many many more like Lexou or Cam or CC or Zahlman or 2WB or Tera...
Let me know if you want a link to your ASM here, but be warned that I'll be judging it.
