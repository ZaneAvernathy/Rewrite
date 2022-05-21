
### Skip Huffman Decompression

In the vanilla game, all text is compressed using Huffman encoding, which is really inconvenient. This hack circumvents the need to do that.

This is made by me (Zane Avernathy)!

This is functionally equivalent to the AntiHuffman hack made by Hextator and Nintenlord.

Shoutouts to the folks who work on the [**FE8 decompilation project**](https://github.com/FireEmblemUniverse/fireemblem8u). I was able to fit things easily without using freespace by doing some optimizations to [**this**](https://github.com/FireEmblemUniverse/fireemblem8u/blob/8ecec19f03e04343c332a2288666cf2c0aba36d2/src/messdecode.c#L53).

### Project structure, tools, etc.

Shoutouts to Stan for everything that he does. ea-dep, lyn, EA stuff, a bunch of makefile stuff (including most of how I'm doing things here), and many other things are all from him.

Shoutouts to CrazyColorz5 for ColorzCore and EA maintenance, along with EA tools and cool wizardry.

Shoutouts to Nintenlord, Cam, Venno, Alfred, Circles, Teq, and many, many more for doing the legwork that got us to where we're at today.

Shoutouts to [**Martin Korth**](https://problemkaputt.de/) for the no$gba debugging emulator and GBATek, which make doing this kind of stuff possible at all.
