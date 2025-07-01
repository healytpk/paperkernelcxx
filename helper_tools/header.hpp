#pragma once

#include <cstdint>         // uint_fast64_t
#include <algorithm>       // lower_bound
#include <string_view>     // string_view

namespace Author_details {

struct Author_Primary {
    std::uint_fast64_t hash;
    char const *name;
};

inline constexpr Author_Primary names_primary[] = {
    { 0x02e8aff2eac9edd0, "Marhsall Clow" },
    { 0x04cdfaff1157608e, "Kyle Kloepper" },
    { 0x055e199230e2288e, "Harold Bott" },
    { 0x0817bbd20d7db399, "Pablo Halpern" },
    { 0x0b4054795cb283d0, "Bryce Adelstein Lelbach" },
    { 0x0c26354fa19f53d6, "Keld Jorn Simonsen" },
    { 0x0c439bdd4527656f, "Corentin Jabot" },
    { 0x0de5e29c00a8ba97, "Michael Spear" },
    { 0x0fb073bb24b207c3, "Geoffrey Romer" },
    { 0x10b16af2fd5d7787, "Jesse Perla" },
    { 0x10bec4decfd721bf, "Peter Gottschling" },
    { 0x10c55f64612fe40f, "Nathan Wilson" },
    { 0x130f69d461f76d4b, "Peter Sommerlad" },
    { 0x148ba671e0a051c9, "David Svoboda" },
    { 0x14d8793696f96fd4, "Robert Geva" },
    { 0x156e307fec38370f, "Robert Kawulak" },
    { 0x15a1fd456d020fa7, "Athanasios (Nasos) Iliopoulos" },
    { 0x17ddd3a8f2972122, "Dietmar K\\u00FChl" },
    { 0x1997d7b281796a7a, "Hans J. Boehm" },
    { 0x1a67090e9e7679b9, "Jonathan Coe" },
    { 0x1bcfe8567346f869, "ADAM David Alan Martin" },
    { 0x1dea36f4641d6b4c, "Nat Goodspeed" },
    { 0x1ebf3f1b14304c4e, "Jonathan Caves" },
    { 0x216f4d599f477cf0, "Glen Joseph Fernandes" },
    { 0x22ab746ff00ab9b3, "Arch D. Robison" },
    { 0x230487cd58b89f5f, "Sylvain Pion" },
    { 0x232852610591dff9, "Stefanus Du Toit" },
    { 0x238505804ec90c68, "Beman Dawes" },
    { 0x24532972f8b374dc, "Bronek Kozicki" },
    { 0x2689742a5ad8e1e7, "M. J. Kronenburg" },
    { 0x268c23a80acc0967, "Jean-Paul Rigault" },
    { 0x28e5e871a0d116d2, "Michael Spertus" },
    { 0x29d113cf721bf621, "J.C. van Winkel" },
    { 0x2c286cebe0b7bde5, "Thomas P. K. Healy" },
    { 0x3100ffb68dfd0a98, "David Miller" },
    { 0x31260698e2b3d7d0, "R. Tim Song" },
    { 0x32bb0980a8ba9df8, "Chris Mysen" },
    { 0x32d5f95cf52964ae, "Staffan Tjernstrom" },
    { 0x3951c22347f227bf, "Duncan P. N. Exon Smith" },
    { 0x3b6a4f75e7ad64d7, "Faisal Vali" },
    { 0x3c6f5823061d807f, "Jeremy Siek" },
    { 0x3d2389c165d38f83, "Nicolas Manual Morales" },
    { 0x3d2d38a87e754212, "John Lakos" },
    { 0x3e8e28ad613274b8, "Anton Malakhov" },
    { 0x403bd539ed57567d, "Jens Maurer" },
    { 0x4124c9c266218c9d, "Alberto Ganesh Barbati" },
    { 0x431b4376c9613e8b, "James Widman" },
    { 0x4327f29e98c57901, "Alan Talbot" },
    { 0x44a90b0ba38c77e3, "R. Martinho Fernandes" },
    { 0x461433a8062490f0, "Charles Hogg" },
    { 0x461f5158d9b5e620, "Alisdair Meredith" },
    { 0x49092381e44b632a, "Bejnamin Kosnik" },
    { 0x49e422c4c07f7353, "Matthew Austern" },
    { 0x49f3fc43721b6ee6, "Jose Daniel Garcia Sanchez" },
    { 0x4ba61bab2cc0ff22, "Jarrad J. Waterloo" },
    { 0x4cbdc280464dcb21, "Joaqu\\u00EDn M\\u00AA L\\u00F3pez Mu\\u00F1oz" },
    { 0x4ce1efb7e90714f1, "Chandler Carruth" },
    { 0x4d8f982689eab038, "Micha\\u0142 Dominiak" },
    { 0x4f9f7ee8ff20a650, "Oliver Kowalke" },
    { 0x5229ea192a285d64, "Mark Batty" },
    { 0x52bd5b2357320cb7, "Oliver J. Rosten" },
    { 0x56eff24aad0d0976, "Yonggang Li" },
    { 0x573278b0a46e71da, "Aleksandar Fabijanic" },
    { 0x573e1ce78c24a261, "Olivier Giroux" },
    { 0x5977d66f492e5684, "Douglas Gregor" },
    { 0x5aee4a5563cc2ccd, "Stephan T. Lavavej" },
    { 0x5c3c7c0b73bf2c01, "Daniel Kr\\u00FCgler" },
    { 0x5e9cb9c37c12695b, "Jonathan Wakely" },
    { 0x5ec8ebced0bbcdb3, "Nick Stoughton" },
    { 0x60394855978bb91c, "Jose Daniel Garcia" },
    { 0x61a3e7d458e47bc4, "Vicente J. Botet Escriba" },
    { 0x6261250e139c09bd, "Marc Paterno" },
    { 0x6446ab32fee192a0, "Gabriel Dos Reis" },
    { 0x64ade448b6a0f195, "Hubert S.K. Tong" },
    { 0x6548d78780805349, "Jim Radigan" },
    { 0x657a1eecf96d6950, "Thorsten Ottosen" },
    { 0x65852a64a9416314, "Niklas Gustafsson" },
    { 0x65da504836ddfafa, "Robert Klarer" },
    { 0x6691c1bf88053658, "Paul E. McKenney" },
    { 0x6cf243fe299cda73, "Alexei Zakharov" },
    { 0x6d2fa31dfdf24f7c, "Daniel Auresco" },
    { 0x6d65778ae11487b1, "Kevlin Henney" },
    { 0x6e248cd85c2b808e, "S. Davis Herring" },
    { 0x6e9a6fca4adef065, "Thomas Witt" },
    { 0x700540dc95347179, "Alasdair Mackintosh" },
    { 0x7249a6040aff8b4e, "Klemans Morgenstern" },
    { 0x725b75701483ea24, "Detlef Vollmann" },
    { 0x743b8ea40797a559, "Neal Meyer" },
    { 0x74e52b64f7ad220b, "Ville Voutilainen" },
    { 0x7b0f9f1596541f30, "J. Stephen Adamczyk" },
    { 0x7b29604307fd1df5, "Michael L. Scott" },
    { 0x7d929c4dbaee8e18, "John H. Spicer" },
    { 0x7e003a6a61ef97a2, "Andrzej Krzemie\\u0144ski" },
    { 0x7f1140f57893233b, "Rani Sharoni" },
    { 0x7faea3897de58efc, "Thomas Richard William Scogland" },
    { 0x81519ddf73923d42, "Daniel Sunderland" },
    { 0x818c72754177ebc5, "Roger Orr" },
    { 0x81d4966d7ec7227b, "Pavol Droba" },
    { 0x822daba75c1e68ed, "Andrew Tomazos" },
    { 0x82b377c5c27d2c7e, "Scott Owens" },
    { 0x83dc98100934fa2f, "Ali Sezgin" },
    { 0x83f907e249e9e323, "Thomas Plum" },
    { 0x843f611c7e4d50a8, "Michael B. McLaughlin" },
    { 0x86cc9702f73474d5, "Peter Sewell" },
    { 0x8766cad0f4a5344d, "Pete Becker" },
    { 0x87fe8e674edba8a3, "Robert Mill" },
    { 0x88198aff7547ab4b, "Nathan Myers" },
    { 0x8a1ceaaf70a4e70a, "James Reus" },
    { 0x8a63b4caf4a1d0a9, "John Maddock" },
    { 0x8c10819a36debb83, "Fernando Cacciola" },
    { 0x8d6347f8bcd5d374, "Thomas W Rodgers" },
    { 0x8ed63783c60b8195, "Hartmut Kaiser" },
    { 0x91037530f0a41997, "Vinnie Falco" },
    { 0x93994571cf45ca6a, "Francis Glassborow" },
    { 0x94d78519fba73eaf, "Jeff Garland" },
    { 0x98e39a5e6c3bb2a0, "Victor Luchangco" },
    { 0x990e9f839c4e6bc1, "Michael Florian Hava" },
    { 0x995da75423844cdc, "David Abrahams" },
    { 0x99941a272712a8c9, "Lawrence Crowl" },
    { 0x99b94032d2aae1b5, "Michael J. Voss" },
    { 0x9b0fd1d5b9672135, "Mingo Gill" },
    { 0x9b9cff49638b98cc, "Erich Keane" },
    { 0x9cabd1f7b5847904, "Bjarne Stroustrup" },
    { 0x9cd9c134011bd8fd, "Alex Naumann" },
    { 0x9e02a55e97ba7a0b, "Attila (Farkas) Feh\\u00E9r" },
    { 0x9f842a5c048c35fc, "Paul A Bristow" },
    { 0xa37ec755375a74b9, "Ren\\u00E9 Rivera" },
    { 0xa7577a774f156779, "Hal Finkel" },
    { 0xa8135e8f4eefc36b, "D. M. Berris" },
    { 0xa8b3f47563dacc81, "Andrew Sutton" },
    { 0xa9881c28f5255d7f, "Clark Nelson" },
    { 0xaa1837970dcef304, "Daniel F. Gutson" },
    { 0xaac25077793a8128, "Anthony Williams." },
    { 0xab4e32ed43ddd632, "Stephen D. Clamage" },
    { 0xad3891cecc9a9fbf, "Howard E. Hinnant" },
    { 0xae6ea42b9548a427, "Torvald Riegel" },
    { 0xaf87ff89bc80e138, "Matthew Bentley" },
    { 0xb064cfb0158434ac, "Niels Dekker" },
    { 0xb2449af7a471753a, "William M. (Mike) Miller" },
    { 0xb4189c0bc10e8abe, "Artur Laksberg" },
    { 0xb4511b81a89dbef6, "Jamie Allsop" },
    { 0xb4f67bb6106b1273, "Logan R. Smith" },
    { 0xb72e4a71cf253043, "Nick Maclaren" },
    { 0xb7cdf8ef8f517f3e, "Nina Dinka Ranns" },
    { 0xb8f0dbb873f36a6a, "Jeff Preshing" },
    { 0xb9904bd60c5dd4e3, "Andrei Alexandrescu" },
    { 0xbbf7de7e54564986, "Andrew Lumsdaine" },
    { 0xbc2e9b83ff013b26, "Yoaodan Zhang" },
    { 0xbca9c539594d60bd, "P.J. Plauger" },
    { 0xbd550f6ea434959b, "Mark Hall" },
    { 0xbee424d92d757479, "Maged M. Michael" },
    { 0xbf1fcdc45ad9b8d0, "Carter H. Edwards" },
    { 0xc3949db4a7de7616, "Daryle Walker" },
    { 0xc769a7847095d10e, "Yehezkel Bernat" },
    { 0xc9473d894a03fd76, "Eric A Niebler" },
    { 0xc9a22f7658a582f7, "Michael Wong" },
    { 0xc9aa60564fa25eab, "Gor Nishanov" },
    { 0xcd1bd8c64e70d104, "Bill Seymour" },
    { 0xce7814743706acda, "Alexander Beels" },
    { 0xce7e9edf5d6ed2fd, "Herb Sutter" },
    { 0xcf0deb348e770253, "James Dennett" },
    { 0xd233dac37a922af7, "Cleiton Santoia Silva" },
    { 0xd6447185afb47e75, "Daveed Vandevoorde" },
    { 0xd683b8e9cc5d4dab, "Agust\\u00EDn Berg\\u00E9" },
    { 0xd8832389526dc573, "Jeff Snyder" },
    { 0xdaa1a874500adf2b, "Christian Kaeser" },
    { 0xdc990e4addef3ebe, "Jason Zink" },
    { 0xe18975b3566476d9, "Jason Merrill" },
    { 0xe1a58d1c1590e626, "Andrew L. Sandoval" },
    { 0xe296ad8469021886, "Sean Parent" },
    { 0xe40b85466eeb77ab, "Edward J. Rosten" },
    { 0xe56cb0388403f753, "Phillip Ratzloff" },
    { 0xe580a2290934a7f4, "Rajan Bhakta" },
    { 0xe75a72085efc4b42, "Alan Jeffrey" },
    { 0xe7964245d921024b, "Walter E. Brown" },
    { 0xe9797737da45f716, "Tony Van Eerd" },
    { 0xeefea065f55057a5, "Botond Ballo" },
    { 0xefe4636913b0a112, "Fabio Fracassi" },
    { 0xf052f8c58acf1fe7, "David Chisnall" },
    { 0xf0d768366c92bc82, "Jeffrey Yasskin" },
    { 0xf24abf7b810381aa, "Jared Hoberock Lee Howes" },
    { 0xf47de4cead7d584e, "Simon Brand" },
    { 0xf4b720cb50f13849, "Daisy S. Hollman" },
    { 0xf5456f8dca31b03e, "Hannes Hauswedell " },
    { 0xf5f780a5748d8c42, "Jo\\u00EBl Falcou" },
    { 0xf69d2585fca84029, "Christopher Kohlhoff" },
    { 0xf800fbe073e58d28, "Peter Dimov" },
    { 0xf84707ca5ef0ce13, "Nicolai Josuttis" },
    { 0xfb9ab70260e6a086, "Jared Hoberock" },
    { 0xfeb354e6e34d0b9a, "Lois Goldthwaite" },
    { 0xff3af99c2ba154af, "Paul M. Bendixen" }
};

struct Author_Alternative {
    std::uint_fast64_t hash;
    char const *name;
    std::uint_fast64_t primary;
};

inline constexpr Author_Alternative names_alternative[] = {
    { 0x000c387a9b3fe3ee, "Paul Bendixen", 0xff3af99c2ba154af }, // primary = "Paul M. Bendixen"
    { 0x021894aad7e13534, "J. Garland", 0x94d78519fba73eaf }, // primary = "Jeff Garland"
    { 0x02c136680452beff, "Yihe Li", 0x56eff24aad0d0976 }, // primary = "Yonggang Li"
    { 0x03090b750cb7b813, "F. Vali", 0x3b6a4f75e7ad64d7 }, // primary = "Faisal Vali"
    { 0x044e39284f516697, "Georey Romer", 0x0fb073bb24b207c3 }, // primary = "Geoffrey Romer"
    { 0x04ea444b6178c641, "Oliver Rosten", 0x52bd5b2357320cb7 }, // primary = "Oliver J. Rosten"
    { 0x0675a6c94ff9ca99, "H. Tong", 0x64ade448b6a0f195 }, // primary = "Hubert S.K. Tong"
    { 0x07facb68de62085c, "F. Cacciola", 0x8c10819a36debb83 }, // primary = "Fernando Cacciola"
    { 0x0aa55766756698f1, "B. Ballo", 0xeefea065f55057a5 }, // primary = "Botond Ballo"
    { 0x0accac2e7606acde, "A. Williams", 0xaac25077793a8128 }, // primary = "Anthony Williams."
    { 0x0bc7ae5d3e1af0c5, "David S. Hollman", 0xf4b720cb50f13849 }, // primary = "Daisy S. Hollman"
    { 0x0c439bdd4527656f, "Corentin jabot", 0x0c439bdd4527656f }, // primary = "Corentin Jabot"
    { 0x0c93588d9f55ba6f, "Eric Niebler", 0xc9473d894a03fd76 }, // primary = "Eric A Niebler"
    { 0x0d62748cb371cc47, "A. Martin", 0x1bcfe8567346f869 }, // primary = "ADAM David Alan Martin"
    { 0x0e621bbc6c9f493f, "Joel FALCOU", 0xf5f780a5748d8c42 }, // primary = "Jo\\u00EBl Falcou"
    { 0x0efc7d28a091942e, "V. Escriba", 0x61a3e7d458e47bc4 }, // primary = "Vicente J. Botet Escriba"
    { 0x0efc7d28a091942e, "V.  Escriba", 0x61a3e7d458e47bc4 }, // primary = "Vicente J. Botet Escriba"
    { 0x0f78ebe52bf24582, "Hans Boehm", 0x1997d7b281796a7a }, // primary = "Hans J. Boehm"
    { 0x0fa97f66ca34e4e8, "Rene Rivera", 0xa37ec755375a74b9 }, // primary = "Ren\\u00E9 Rivera"
    { 0x1040e2d2bd5b7152, "Steffan Tjernstrom", 0x32d5f95cf52964ae }, // primary = "Staffan Tjernstrom"
    { 0x12c8c32565bc9559, "Lee Howes", 0xf24abf7b810381aa }, // primary = "Jared Hoberock Lee Howes"
    { 0x13133055ef18cbad, "J. Widman", 0x431b4376c9613e8b }, // primary = "James Widman"
    { 0x13bb7b9f08b0ee7d, "Matt Austern", 0x49e422c4c07f7353 }, // primary = "Matthew Austern"
    { 0x15dbae3a1f376aba, "B. Kosnik", 0x49092381e44b632a }, // primary = "Bejnamin Kosnik"
    { 0x16eed034544dcb73, "R. Mill", 0x87fe8e674edba8a3 }, // primary = "Robert Mill"
    { 0x1879bc1fa2946658, "A. G. Barbati", 0x4124c9c266218c9d }, // primary = "Alberto Ganesh Barbati"
    { 0x1997d7b281796a7a, "Hans-J. Boehm", 0x1997d7b281796a7a }, // primary = "Hans J. Boehm"
    { 0x19ef4161ddea22bd, "M. Paterno", 0x6261250e139c09bd }, // primary = "Marc Paterno"
    { 0x1a0df1ca5f7ae4c2, "W. Brown", 0xe7964245d921024b }, // primary = "Walter E. Brown"
    { 0x1bcfe8567346f869, "Adam David Alan Martin", 0x1bcfe8567346f869 }, // primary = "ADAM David Alan Martin"
    { 0x1daf69e37ff76c39, "J. Caves", 0x1ebf3f1b14304c4e }, // primary = "Jonathan Caves"
    { 0x1f9b42705d420fed, "A. Beels", 0xce7814743706acda }, // primary = "Alexander Beels"
    { 0x1fc503278861f55a, "V. Falco", 0x91037530f0a41997 }, // primary = "Vinnie Falco"
    { 0x1ff11c6be0fb6849, "F. Fracassi", 0xefe4636913b0a112 }, // primary = "Fabio Fracassi"
    { 0x203d61c6806a6d00, "Tim Song", 0x31260698e2b3d7d0 }, // primary = "R. Tim Song"
    { 0x2142467f1082cbd7, "Michal Dominiak", 0x4d8f982689eab038 }, // primary = "Micha\\u0142 Dominiak"
    { 0x21fde6fda6ada8cf, "N. Goodspeed", 0x1dea36f4641d6b4c }, // primary = "Nat Goodspeed"
    { 0x224b417603cd26e9, "C. Carruth", 0x4ce1efb7e90714f1 }, // primary = "Chandler Carruth"
    { 0x2265fadb83d5ef76, "N. Gustafsson", 0x65852a64a9416314 }, // primary = "Niklas Gustafsson"
    { 0x232852610591dff9, "Stefanus du Toit", 0x232852610591dff9 }, // primary = "Stefanus Du Toit"
    { 0x233c02ec037de784, "F. Glassborow", 0x93994571cf45ca6a }, // primary = "Francis Glassborow"
    { 0x2528b5e9be264349, "A. Sutton", 0xa8b3f47563dacc81 }, // primary = "Andrew Sutton"
    { 0x25f2faffef822e84, "R. Geva", 0x14d8793696f96fd4 }, // primary = "Robert Geva"
    { 0x2689742a5ad8e1e7, "M.J. Kronenburg", 0x2689742a5ad8e1e7 }, // primary = "M. J. Kronenburg"
    { 0x273bbdc3c630833a, "Jarrad Waterloo", 0x4ba61bab2cc0ff22 }, // primary = "Jarrad J. Waterloo"
    { 0x28fd329bfc7913cc, "O. Kowalke", 0x4f9f7ee8ff20a650 }, // primary = "Oliver Kowalke"
    { 0x29105b89ef9141af, "Micheal Wong", 0xc9a22f7658a582f7 }, // primary = "Michael Wong"
    { 0x292befe90e0bec34, "A. Meredith", 0x461f5158d9b5e620 }, // primary = "Alisdair Meredith"
    { 0x29d113cf721bf621, "JC van Winkel", 0x29d113cf721bf621 }, // primary = "J.C. van Winkel"
    { 0x29d113cf721bf621, "JC van WInkel", 0x29d113cf721bf621 }, // primary = "J.C. van Winkel"
    { 0x29fddc83d1eb7df8, "Oliver Giroux", 0x573e1ce78c24a261 }, // primary = "Olivier Giroux"
    { 0x2c61aaf15d874377, "Yehuda Bernat", 0xc769a7847095d10e }, // primary = "Yehezkel Bernat"
    { 0x2eaa322751403b7a, "Benjamin Kosnik", 0x49092381e44b632a }, // primary = "Bejnamin Kosnik"
    { 0x2eb4b709b4beb652, "C. Mysen", 0x32bb0980a8ba9df8 }, // primary = "Chris Mysen"
    { 0x2fb307b89d1e3069, "T. Plum", 0x83f907e249e9e323 }, // primary = "Thomas Plum"
    { 0x3065e5cec8a4b8f1, "Tom Plum", 0x83f907e249e9e323 }, // primary = "Thomas Plum"
    { 0x3222de7e81683d80, "H. Finkel", 0xa7577a774f156779 }, // primary = "Hal Finkel"
    { 0x3251c39ec1a44d38, "Daisy Hollman", 0xf4b720cb50f13849 }, // primary = "Daisy S. Hollman"
    { 0x328233a64c5a93c0, "S. Adamczyk", 0x7b0f9f1596541f30 }, // primary = "J. Stephen Adamczyk"
    { 0x339b775fcda86e5e, "A. Talbot", 0x4327f29e98c57901 }, // primary = "Alan Talbot"
    { 0x34eee8cd1f541c04, "William M. Miller", 0xb2449af7a471753a }, // primary = "William M. (Mike) Miller"
    { 0x35483fa23a34ab0c, "A. Berg\\u00E9", 0xd683b8e9cc5d4dab }, // primary = "Agust\\u00EDn Berg\\u00E9"
    { 0x3951c22347f227bf, "Duncan P.N. Exon Smith", 0x3951c22347f227bf }, // primary = "Duncan P. N. Exon Smith"
    { 0x3991989a38ece510, "Doug Gregor", 0x5977d66f492e5684 }, // primary = "Douglas Gregor"
    { 0x3a18806765f50266, "C. Edwards", 0xbf1fcdc45ad9b8d0 }, // primary = "Carter H. Edwards"
    { 0x3ec853d36766f60f, "Athanasios Iliopoulos", 0x15a1fd456d020fa7 }, // primary = "Athanasios (Nasos) Iliopoulos"
    { 0x3ef0b34066e617c1, "Artus Laksberg", 0xb4189c0bc10e8abe }, // primary = "Artur Laksberg"
    { 0x3f4ecd5220bf9655, "A. Fabijanic", 0x573278b0a46e71da }, // primary = "Aleksandar Fabijanic"
    { 0x424bb24d611a56c1, "H. Hinnant", 0xad3891cecc9a9fbf }, // primary = "Howard E. Hinnant"
    { 0x4316afa74db48e50, "Eric Keane", 0x9b9cff49638b98cc }, // primary = "Erich Keane"
    { 0x436cab5b554049fa, "K. Kloepper", 0x04cdfaff1157608e }, // primary = "Kyle Kloepper"
    { 0x44bef0bb7e9075ba, "D. Berris", 0xa8135e8f4eefc36b }, // primary = "D. M. Berris"
    { 0x46a33f8173db6821, "Glen Fernandes", 0x216f4d599f477cf0 }, // primary = "Glen Joseph Fernandes"
    { 0x47a8509fe61186bb, "P. Becker", 0x8766cad0f4a5344d }, // primary = "Pete Becker"
    { 0x4b5a2b2bd9c45f3f, "Richard Smith", 0xb4f67bb6106b1273 }, // primary = "Logan R. Smith"
    { 0x4ba2c9a3f98c1498, "M. Spertus", 0x28e5e871a0d116d2 }, // primary = "Michael Spertus"
    { 0x4ba61bab2cc0ff22, "Jarrad J Waterloo", 0x4ba61bab2cc0ff22 }, // primary = "Jarrad J. Waterloo"
    { 0x4c0e54d888eb6018, "John Spicer", 0x7d929c4dbaee8e18 }, // primary = "John H. Spicer"
    { 0x4ce3219de86f8d76, "Michael McLaughlin", 0x843f611c7e4d50a8 }, // primary = "Michael B. McLaughlin"
    { 0x4daf61a400c9f309, "P. Sommerlad", 0x130f69d461f76d4b }, // primary = "Peter Sommerlad"
    { 0x4fc766833799b133, "P. Droba", 0x81d4966d7ec7227b }, // primary = "Pavol Droba"
    { 0x4fd59d872636be8a, "R. Klarer", 0x65da504836ddfafa }, // primary = "Robert Klarer"
    { 0x5212586d1812911b, "T. Riegel", 0xae6ea42b9548a427 }, // primary = "Torvald Riegel"
    { 0x5238b8a045bef53b, "P. Sewell", 0x86cc9702f73474d5 }, // primary = "Peter Sewell"
    { 0x545546ab8d7ac120, "R. Smith", 0xb4f67bb6106b1273 }, // primary = "Logan R. Smith"
    { 0x5474a53a935f2813, "Chris Kohlhoff", 0xf69d2585fca84029 }, // primary = "Christopher Kohlhoff"
    { 0x54b43a2bcce2af13, "Alberto Barbati", 0x4124c9c266218c9d }, // primary = "Alberto Ganesh Barbati"
    { 0x565503cce62399e9, "J. Dennett", 0xcf0deb348e770253 }, // primary = "James Dennett"
    { 0x573d00796c8ea1e1, "P. McKenney", 0x6691c1bf88053658 }, // primary = "Paul E. McKenney"
    { 0x579c0dfa10aa488d, "J. Mu\\u00F1oz", 0x4cbdc280464dcb21 }, // primary = "Joaqu\\u00EDn M\\u00AA L\\u00F3pez Mu\\u00F1oz"
    { 0x58f035590ed4f8ed, "A. Feh\\u00E9r", 0x9e02a55e97ba7a0b }, // primary = "Attila (Farkas) Feh\\u00E9r"
    { 0x59447c5407fbed29, "Gregor", 0x5977d66f492e5684 }, // primary = "Douglas Gregor"
    { 0x59ac462ca206ab0c, "David Hollman", 0xf4b720cb50f13849 }, // primary = "Daisy S. Hollman"
    { 0x5a6d9857c827d7c2, "A. Sandoval", 0xe1a58d1c1590e626 }, // primary = "Andrew L. Sandoval"
    { 0x5ac72e9c70de4f13, "R. Sharoni", 0x7f1140f57893233b }, // primary = "Rani Sharoni"
    { 0x5c90098f50e9c2f5, "N. Wilson", 0x10c55f64612fe40f }, // primary = "Nathan Wilson"
    { 0x5d6e6e299303bcf0, "L. Crowl", 0x99941a272712a8c9 }, // primary = "Lawrence Crowl"
    { 0x5ec32126bcd2beef, "K. Henney", 0x6d65778ae11487b1 }, // primary = "Kevlin Henney"
    { 0x60a333d182f93ac2, "Marshall Clow", 0x02e8aff2eac9edd0 }, // primary = "Marhsall Clow"
    { 0x611c66df35fb565e, "H. Carter Edwards", 0xbf1fcdc45ad9b8d0 }, // primary = "Carter H. Edwards"
    { 0x61caad40a0963a17, "D. Svoboda", 0x148ba671e0a051c9 }, // primary = "David Svoboda"
    { 0x64a1d00c95123b23, "C. Silva", 0xd233dac37a922af7 }, // primary = "Cleiton Santoia Silva"
    { 0x650f0e0e9b71cec2, "Maged Michael", 0xbee424d92d757479 }, // primary = "Maged M. Michael"
    { 0x679da41c72858e8f, "A. Laksberg", 0xb4189c0bc10e8abe }, // primary = "Artur Laksberg"
    { 0x67b47b686a15f533, "Daniel Garcia", 0x60394855978bb91c }, // primary = "Jose Daniel Garcia"
    { 0x67bb2b4b5ddc8f87, "Bryce Lelbach", 0x0b4054795cb283d0 }, // primary = "Bryce Adelstein Lelbach"
    { 0x6b78c46e285f4951, "Nicolas Morales", 0x3d2389c165d38f83 }, // primary = "Nicolas Manual Morales"
    { 0x6d2e48faa9f9f849, "D. Walker", 0xc3949db4a7de7616 }, // primary = "Daryle Walker"
    { 0x706269341da36b7e, "Howard Hinnant", 0xad3891cecc9a9fbf }, // primary = "Howard E. Hinnant"
    { 0x711221d01bb88709, "A. Alexandrescu", 0xb9904bd60c5dd4e3 }, // primary = "Andrei Alexandrescu"
    { 0x71cf62061dada533, "J. Radigan", 0x6548d78780805349 }, // primary = "Jim Radigan"
    { 0x72485f3c3db0b843, "B. Dawes", 0x238505804ec90c68 }, // primary = "Beman Dawes"
    { 0x725ba5c50a9a2e92, "D. Kr\\u00FCgler", 0x5c3c7c0b73bf2c01 }, // primary = "Daniel Kr\\u00FCgler"
    { 0x7371ed1353845a65, "A. Tomazos", 0x822daba75c1e68ed }, // primary = "Andrew Tomazos"
    { 0x75f62066a547aba4, "David Vandevoorde", 0xd6447185afb47e75 }, // primary = "Daveed Vandevoorde"
    { 0x7645d23270d869c0, "Dan Sunderland", 0x81519ddf73923d42 }, // primary = "Daniel Sunderland"
    { 0x76642593e9899c39, "M. Michael", 0xbee424d92d757479 }, // primary = "Maged M. Michael"
    { 0x76d775222bbbfa0b, "J. Daniel Garcia", 0x60394855978bb91c }, // primary = "Jose Daniel Garcia"
    { 0x76eb86b7ba731a80, "A. Sezgin", 0x83dc98100934fa2f }, // primary = "Ali Sezgin"
    { 0x79003c1eb5f4b61f, "J. Adamczyk", 0x7b0f9f1596541f30 }, // primary = "J. Stephen Adamczyk"
    { 0x7d384ad0290e4a8c, "N. Maclaren", 0xb72e4a71cf253043 }, // primary = "Nick Maclaren"
    { 0x7dc954ed88da9129, "Glen J. Fernandes", 0x216f4d599f477cf0 }, // primary = "Glen Joseph Fernandes"
    { 0x7df48f8ca24a02f6, "D. Vandevoorde", 0xd6447185afb47e75 }, // primary = "Daveed Vandevoorde"
    { 0x7e0db4ab179b8bc8, "H. Sutter", 0xce7e9edf5d6ed2fd }, // primary = "Herb Sutter"
    { 0x7f4b21eeafa5dc28, "H. Boehm", 0x1997d7b281796a7a }, // primary = "Hans J. Boehm"
    { 0x7f4b21eeafa5dc28, "H.. Boehm", 0x1997d7b281796a7a }, // primary = "Hans J. Boehm"
    { 0x80a7ae0c53af815a, "Sy Brand", 0xf47de4cead7d584e }, // primary = "Simon Brand"
    { 0x80d2a11e0f60fa33, "Nicolia Josuttis", 0xf84707ca5ef0ce13 }, // primary = "Nicolai Josuttis"
    { 0x812c659c050f6047, "W. Miller", 0xb2449af7a471753a }, // primary = "William M. (Mike) Miller"
    { 0x81f49a5fb670a516, "Harry Bott", 0x055e199230e2288e }, // primary = "Harold Bott"
    { 0x83614637806d11b9, "S. du Toit", 0x232852610591dff9 }, // primary = "Stefanus Du Toit"
    { 0x83614637806d11b9, "S. Du Toit", 0x232852610591dff9 }, // primary = "Stefanus Du Toit"
    { 0x83de61a56a600919, "Arch Robison", 0x22ab746ff00ab9b3 }, // primary = "Arch D. Robison"
    { 0x843f611c7e4d50a8, "Michael B McLaughlin", 0x843f611c7e4d50a8 }, // primary = "Michael B. McLaughlin"
    { 0x84887b77791ae9b9, "N. Meyer", 0x743b8ea40797a559 }, // primary = "Neal Meyer"
    { 0x8524ebba80defd76, "N. Stoughton", 0x5ec8ebced0bbcdb3 }, // primary = "Nick Stoughton"
    { 0x85a864433a5117ca, "J. Snyder", 0xd8832389526dc573 }, // primary = "Jeff Snyder"
    { 0x878f4448c1230dfe, "A. Lumsdaine", 0xbbf7de7e54564986 }, // primary = "Andrew Lumsdaine"
    { 0x87c211cbc82988f3, "Kozicki", 0x24532972f8b374dc }, // primary = "Bronek Kozicki"
    { 0x8832ac33be0c34e0, "R. Rivera", 0xa37ec755375a74b9 }, // primary = "Ren\\u00E9 Rivera"
    { 0x893587bfa8d01e2f, "T. Witt", 0x6e9a6fca4adef065 }, // primary = "Thomas Witt"
    { 0x8ae074dd84266b41, "G. Dos Reis", 0x6446ab32fee192a0 }, // primary = "Gabriel Dos Reis"
    { 0x8b1d3f8adb90d338, "S. Pion", 0x230487cd58b89f5f }, // primary = "Sylvain Pion"
    { 0x8b8b4525d212666f, "J. Zink", 0xdc990e4addef3ebe }, // primary = "Jason Zink"
    { 0x8bc89b51a464e75d, "M. Wong", 0xc9a22f7658a582f7 }, // primary = "Michael Wong"
    { 0x8c086051843dbc83, "J. Preshing", 0xb8f0dbb873f36a6a }, // primary = "Jeff Preshing"
    { 0x8d238836e07b2442, "J. Allsop", 0xb4511b81a89dbef6 }, // primary = "Jamie Allsop"
    { 0x8df3074bd0b0194d, "A. Krzemie\\u0144ski", 0x7e003a6a61ef97a2 }, // primary = "Andrzej Krzemie\\u0144ski"
    { 0x8eced2231e7383db, "G. Romer", 0x0fb073bb24b207c3 }, // primary = "Geoffrey Romer"
    { 0x8f85ec7c65638146, "J. Wakely", 0x5e9cb9c37c12695b }, // primary = "Jonathan Wakely"
    { 0x90a2fa70b84700a6, "A. Robison", 0x22ab746ff00ab9b3 }, // primary = "Arch D. Robison"
    { 0x924adc8454e6755d, "E. Niebler", 0xc9473d894a03fd76 }, // primary = "Eric A Niebler"
    { 0x9267ab8841ed723b, "N. Dekker", 0xb064cfb0158434ac }, // primary = "Niels Dekker"
    { 0x942d42bf79a301cd, "Nico Josuttis", 0xf84707ca5ef0ce13 }, // primary = "Nicolai Josuttis"
    { 0x96f6c4e7fff15203, "Geoff Romer", 0x0fb073bb24b207c3 }, // primary = "Geoffrey Romer"
    { 0x981e1b7aae505932, "Phil Ratzloff", 0xe56cb0388403f753 }, // primary = "Phillip Ratzloff"
    { 0x9ae4650e62ae3f2d, "P. Plauger", 0xbca9c539594d60bd }, // primary = "P.J. Plauger"
    { 0x9b53933b708acca9, "Keld Simonsen", 0x0c26354fa19f53d6 }, // primary = "Keld Jorn Simonsen"
    { 0x9c75a16122d1578d, "J. Siek", 0x3c6f5823061d807f }, // primary = "Jeremy Siek"
    { 0x9cce3acd8b02d55a, "J. D. Garcia", 0x60394855978bb91c }, // primary = "Jose Daniel Garcia"
    { 0x9d039b0e38905f82, "J. Merrill", 0xe18975b3566476d9 }, // primary = "Jason Merrill"
    { 0x9d24c945cbb23d47, "J. Maurer", 0x403bd539ed57567d }, // primary = "Jens Maurer"
    { 0x9ed94dcdf2d3fd8d, "Nina Ranns", 0xb7cdf8ef8f517f3e }, // primary = "Nina Dinka Ranns"
    { 0x9f349e7909f0e4a5, "J. Spicer", 0x7d929c4dbaee8e18 }, // primary = "John H. Spicer"
    { 0x9fb9cb55f3aec6e2, "Jim Reus", 0x8a1ceaaf70a4e70a }, // primary = "James Reus"
    { 0xa17ed899076cb267, "L. Goldthwaite", 0xfeb354e6e34d0b9a }, // primary = "Lois Goldthwaite"
    { 0xa238b6c2c4656ee3, "C. Nelson", 0xa9881c28f5255d7f }, // primary = "Clark Nelson"
    { 0xa2c3a114fb75faf5, "J.-P. Rigault", 0x268c23a80acc0967 }, // primary = "Jean-Paul Rigault"
    { 0xa3b0fd1ff112c672, "R. Orr", 0x818c72754177ebc5 }, // primary = "Roger Orr"
    { 0xa4743ea0a7a0569b, "J. Daniel Garcia Sanchez", 0x49f3fc43721b6ee6 }, // primary = "Jose Daniel Garcia Sanchez"
    { 0xa517a113961dca39, "Mike Spertus", 0x28e5e871a0d116d2 }, // primary = "Michael Spertus"
    { 0xa722f106cb45fb1a, "Daniel Gutson", 0xaa1837970dcef304 }, // primary = "Daniel F. Gutson"
    { 0xa8fc87c35e559b96, "Matt Bentley", 0xaf87ff89bc80e138 }, // primary = "Matthew Bentley"
    { 0xa9e78d7499a21fb4, "R. Bhakta", 0xe580a2290934a7f4 }, // primary = "Rajan Bhakta"
    { 0xaa79e7de64d4217e, "J. Coe", 0x1a67090e9e7679b9 }, // primary = "Jonathan Coe"
    { 0xaac25077793a8128, "Anthony Williams", 0xaac25077793a8128 }, // primary = "Anthony Williams."
    { 0xaacf505c66323894, "Chip Hogg", 0x461433a8062490f0 }, // primary = "Charles Hogg"
    { 0xac76e6cf71dc1a9d, "Edward Rosten", 0xe40b85466eeb77ab }, // primary = "Edward J. Rosten"
    { 0xadaf35ae82624463, "TPK Healy", 0x2c286cebe0b7bde5 }, // primary = "Thomas P. K. Healy"
    { 0xae6045c5cace6e92, "B. Stroustrup", 0x9cabd1f7b5847904 }, // primary = "Bjarne Stroustrup"
    { 0xb019bc2bc266d193, "D. Gregor", 0x5977d66f492e5684 }, // primary = "Douglas Gregor"
    { 0xb07f942e4afb6cd5, "Younan Zhang", 0xbc2e9b83ff013b26 }, // primary = "Yoaodan Zhang"
    { 0xb268637093e83e87, "Michael Voss", 0x99b94032d2aae1b5 }, // primary = "Michael J. Voss"
    { 0xb33fa32020bdf33c, "M. Batty", 0x5229ea192a285d64 }, // primary = "Mark Batty"
    { 0xb4830e34c6657509, "Thomas Rodgers", 0x8d6347f8bcd5d374 }, // primary = "Thomas W Rodgers"
    { 0xb5ffed0b50f1a32f, "D. Auresco", 0x6d2fa31dfdf24f7c }, // primary = "Daniel Auresco"
    { 0xb6fe97aa2cfcbca1, "J. Lakos", 0x3d2d38a87e754212 }, // primary = "John Lakos"
    { 0xb859c107466ef5a2, "D. Miller", 0x3100ffb68dfd0a98 }, // primary = "David Miller"
    { 0xba750bad12e1521e, "D. Abrahams", 0x995da75423844cdc }, // primary = "David Abrahams"
    { 0xbaa06d331572b60c, "W. M. Miller", 0xb2449af7a471753a }, // primary = "William M. (Mike) Miller"
    { 0xbaf79043004a1ee8, "J. S. Adamczyk", 0x7b0f9f1596541f30 }, // primary = "J. Stephen Adamczyk"
    { 0xbba8b22d4557a43f, "R. Kawulak", 0x156e307fec38370f }, // primary = "Robert Kawulak"
    { 0xbd7e7d107bfa0e30, "S. Owens", 0x82b377c5c27d2c7e }, // primary = "Scott Owens"
    { 0xbe757685925f61fb, "P. Bristow", 0x9f842a5c048c35fc }, // primary = "Paul A Bristow"
    { 0xbebc4a3be7723e91, "J. Falcou", 0xf5f780a5748d8c42 }, // primary = "Jo\\u00EBl Falcou"
    { 0xbf9d6e2582370f31, "D. Chisnall", 0xf052f8c58acf1fe7 }, // primary = "David Chisnall"
    { 0xc0b7bf32b22dd1ce, "A. Naumann", 0x9cd9c134011bd8fd }, // primary = "Alex Naumann"
    { 0xc436b3633be3e8fd, "T. Ottosen", 0x657a1eecf96d6950 }, // primary = "Thorsten Ottosen"
    { 0xc539260ab1120251, "C. Kaeser", 0xdaa1a874500adf2b }, // primary = "Christian Kaeser"
    { 0xc59c0a34bf4de412, "P. Dimov", 0xf800fbe073e58d28 }, // primary = "Peter Dimov"
    { 0xc6e327577d0a5b52, "G. Nishanov", 0xc9aa60564fa25eab }, // primary = "Gor Nishanov"
    { 0xc86fcd130cdeaeb1, "M. Spear", 0x0de5e29c00a8ba97 }, // primary = "Michael Spear"
    { 0xc97dc02aec1bad80, "M. McLaughlin", 0x843f611c7e4d50a8 }, // primary = "Michael B. McLaughlin"
    { 0xca0b771d678e6c60, "Michael Hava", 0x990e9f839c4e6bc1 }, // primary = "Michael Florian Hava"
    { 0xca68a305759da5b0, "Michael Dominiak", 0x4d8f982689eab038 }, // primary = "Micha\\u0142 Dominiak"
    { 0xcc52517f0877fdab, "P. Halpern", 0x0817bbd20d7db399 }, // primary = "Pablo Halpern"
    { 0xcd254d06ac83205d, "A. Mackintosh", 0x700540dc95347179 }, // primary = "Alasdair Mackintosh"
    { 0xcee45d3628b5e91a, "Tom Scogland", 0x7faea3897de58efc }, // primary = "Thomas Richard William Scogland"
    { 0xd275b0d733362513, "D. Gutson", 0xaa1837970dcef304 }, // primary = "Daniel F. Gutson"
    { 0xd64d5f1cad843073, "B. Kozicki", 0x24532972f8b374dc }, // primary = "Bronek Kozicki"
    { 0xd83615c662ab4733, "J. Perla", 0x10b16af2fd5d7787 }, // primary = "Jesse Perla"
    { 0xda091fb173f72ccc, "A. Zakharov", 0x6cf243fe299cda73 }, // primary = "Alexei Zakharov"
    { 0xda1cf5a7c530a656, "D. Vollmann", 0x725b75701483ea24 }, // primary = "Detlef Vollmann"
    { 0xdadbee0c625c7bc1, "P. Gottschling", 0x10bec4decfd721bf }, // primary = "Peter Gottschling"
    { 0xdaeed16e8b163a23, "A. Jeffrey", 0xe75a72085efc4b42 }, // primary = "Alan Jeffrey"
    { 0xdc440a692a5a3669, "V. Voutilainen", 0x74e52b64f7ad220b }, // primary = "Ville Voutilainen"
    { 0xdd399385cca80ffa, "J. Maddock", 0x8a63b4caf4a1d0a9 }, // primary = "John Maddock"
    { 0xdec162592603b6aa, "Klemens Morgenstern", 0x7249a6040aff8b4e }, // primary = "Klemans Morgenstern"
    { 0xdec21383a3099368, "G. Fernandes", 0x216f4d599f477cf0 }, // primary = "Glen Joseph Fernandes"
    { 0xe0b0cc87dd431459, "Davis Herring", 0x6e248cd85c2b808e }, // primary = "S. Davis Herring"
    { 0xe0d8d34b7b79de70, "H. Kaiser", 0x8ed63783c60b8195 }, // primary = "Hartmut Kaiser"
    { 0xe283979094fbb8d8, "Steve Clamage", 0xab4e32ed43ddd632 }, // primary = "Stephen D. Clamage"
    { 0xe77ca1a8a7348e50, "Carter Edwards", 0xbf1fcdc45ad9b8d0 }, // primary = "Carter H. Edwards"
    { 0xe7827e545cb8eded, "N. Myers", 0x88198aff7547ab4b }, // primary = "Nathan Myers"
    { 0xe7964245d921024b, "Walter E Brown", 0xe7964245d921024b }, // primary = "Walter E. Brown"
    { 0xe7e28c74f597ca62, "Dave Abrahams", 0x995da75423844cdc }, // primary = "David Abrahams"
    { 0xe83c1c5ed768a9fa, "S. Parent", 0xe296ad8469021886 }, // primary = "Sean Parent"
    { 0xe9797737da45f716, "Tony van Eerd", 0xe9797737da45f716 }, // primary = "Tony Van Eerd"
    { 0xea3a4fa8b1c7799d, "J. Yasskin", 0xf0d768366c92bc82 }, // primary = "Jeffrey Yasskin"
    { 0xeab3a3f725717f30, "J. Hoberock", 0xfb9ab70260e6a086 }, // primary = "Jared Hoberock"
    { 0xeb21745cabc1e103, "M. Hall", 0xbd550f6ea434959b }, // primary = "Mark Hall"
    { 0xebb56ca27e9e6c74, "O. Giroux", 0x573e1ce78c24a261 }, // primary = "Olivier Giroux"
    { 0xec231a92150c1daa, "Walter Brown", 0xe7964245d921024b }, // primary = "Walter E. Brown"
    { 0xec66ea004ac17a4d, "Michael Scott", 0x7b29604307fd1df5 }, // primary = "Michael L. Scott"
    { 0xed5f400bffc038d8, "H.-J. Boehm", 0x1997d7b281796a7a }, // primary = "Hans J. Boehm"
    { 0xef13a3f4ad8931a1, "S. Lavavej", 0x5aee4a5563cc2ccd }, // primary = "Stephan T. Lavavej"
    { 0xef535fbbacb37913, "Hubert Tong", 0x64ade448b6a0f195 }, // primary = "Hubert S.K. Tong"
    { 0xef5a84b5a9d039d2, "Jon Caves", 0x1ebf3f1b14304c4e }, // primary = "Jonathan Caves"
    { 0xf03d10b90c8944ef, "A. Malakhov", 0x3e8e28ad613274b8 }, // primary = "Anton Malakhov"
    { 0xf25ac8eeed7a6fbf, "D.S. Hollman", 0xf4b720cb50f13849 }, // primary = "Daisy S. Hollman"
    { 0xf25ac8eeed7a6fbf, "D. S. Hollman", 0xf4b720cb50f13849 }, // primary = "Daisy S. Hollman"
    { 0xf37b86ec1a747cf0, "Marshal Clow", 0x02e8aff2eac9edd0 }, // primary = "Marhsall Clow"
    { 0xf440507abdd94d3b, "B. Seymour", 0xcd1bd8c64e70d104 }, // primary = "Bill Seymour"
    { 0xf5456f8dca31b03e, "Hannes Hauswedell", 0xf5456f8dca31b03e }, // primary = "Hannes Hauswedell "
    { 0xf865e29d9f9f8822, "Alex Fabijanic", 0x573278b0a46e71da }, // primary = "Aleksandar Fabijanic"
    { 0xf8b8f82f2e32f560, "M. Austern", 0x49e422c4c07f7353 }, // primary = "Matthew Austern"
    { 0xf9f0e19a412c282d, "Paul McKenney", 0x6691c1bf88053658 }, // primary = "Paul E. McKenney"
    { 0xfa1b183244235561, "Mungo Gill", 0x9b0fd1d5b9672135 }, // primary = "Mingo Gill"
    { 0xfa9778c010c4e1f5, "V. Luchangco", 0x98e39a5e6c3bb2a0 }, // primary = "Victor Luchangco"
    { 0xfbeca6eba89ab0f1, "Attila Feh\\u00E9r", 0x9e02a55e97ba7a0b }, // primary = "Attila (Farkas) Feh\\u00E9r"
    { 0xfc98c5faa2f3fe13, "Martinho Fernandes", 0x44a90b0ba38c77e3 }, // primary = "R. Martinho Fernandes"
    { 0xfdc9323056f27f6d, "C. Kohlhoff", 0xf69d2585fca84029 }, // primary = "Christopher Kohlhoff"
    { 0xfe0a701e8e89376c, "D. K\\u00FChl", 0x17ddd3a8f2972122 }, // primary = "Dietmar K\\u00FChl"
    { 0xfe974729ae57c30d, "Axel Naumann", 0x9cd9c134011bd8fd }, // primary = "Alex Naumann"
    { 0xffad698bc869312c, "Vandevoorde", 0xd6447185afb47e75 }, // primary = "Daveed Vandevoorde"
};


inline constexpr char tolower_consteval(char c) noexcept
{
    switch (c)
    {
        case 'A': return 'a'; case 'B': return 'b'; case 'C': return 'c'; case 'D': return 'd';
        case 'E': return 'e'; case 'F': return 'f'; case 'G': return 'g'; case 'H': return 'h';
        case 'I': return 'i'; case 'J': return 'j'; case 'K': return 'k'; case 'L': return 'l';
        case 'M': return 'm'; case 'N': return 'n'; case 'O': return 'o'; case 'P': return 'p';
        case 'Q': return 'q'; case 'R': return 'r'; case 'S': return 's'; case 'T': return 't';
        case 'U': return 'u'; case 'V': return 'v'; case 'W': return 'w'; case 'X': return 'x';
        case 'Y': return 'y'; case 'Z': return 'z';
        default:  return c;
    }
}

inline constexpr char letters[] = "abcdefghijklmnopqrstuvwxyz";

inline constexpr std::uint_fast64_t Hash(std::string_view const input) noexcept
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    for ( char const cX : input )
    {
        char const c = tolower_consteval(cX);
        bool is_letter = false;

        for ( char letter : letters )
        {
            if ( letter == c )
            {
                is_letter = true;
                break;
            }
        }

        if ( false == is_letter ) continue;

        h ^= static_cast<std::uint_fast64_t>(c);
        h *= 0x100000001B3;
    }

    return h;
}

} // namespace Author_details

inline constexpr const Author_details::Author_Primary *FindAuthor(char const *const p) noexcept
{
    using namespace Author_details;

    constexpr size_t primary_count = sizeof(names_primary) / sizeof(names_primary[0]);
    constexpr size_t alternative_count = sizeof(names_alternative) / sizeof(names_alternative[0]);

    const std::uint_fast64_t h = Hash(p);

    // Search alternatives with lower_bound
    auto altIt = std::lower_bound(
        names_alternative, names_alternative + alternative_count, h,
        [](const Author_Alternative& a, std::uint_fast64_t value) {
            return a.hash < value;
        });

    if (altIt != names_alternative + alternative_count && altIt->hash == h)
    {
        // Search primary with lower_bound
        auto primIt = std::lower_bound(
            names_primary, names_primary + primary_count, altIt->primary,
            [](const Author_Primary& a, std::uint_fast64_t value) {
                return a.hash < value;
            });

        if (primIt != names_primary + primary_count && primIt->hash == altIt->primary)
            return primIt;

        return nullptr;
    }

    // Search primary with lower_bound
    auto primIt = std::lower_bound(
        names_primary, names_primary + primary_count, h,
        [](const Author_Primary& a, std::uint_fast64_t value) {
            return a.hash < value;
        });

    if (primIt != names_primary + primary_count && primIt->hash == h)
        return primIt;

    return nullptr;
}
