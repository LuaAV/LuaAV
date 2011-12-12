LuaAV: Audiovisual Scripting with Lua
======================================================================
See the copyright information in the file named `COPYRIGHT`.


LuaAV is an integrated programming environment based upon extensions to the Lua programming language to enable the tight real-time integration of computation, time, sound and space.  LuaAV is first and foremost an audiovisual composition environment, but can be used for general Lua scripting and prototyping as well.

Principal developers: Wesley Smith and Graham Wakefield.

LuaAV Website: http://lua-av.mat.ucsb.edu


LuaAV Concept
-----------

LuaAV is a real-time audiovisual scripting environment based around the Lua scripting language and a collection of libraries for sound, graphics, and media protocols. The goal of LuaAV is to serve as a computational platform for creative exploration that is both fluid and generative. As with the Lua language, the design strategies for LuaAV favor mechanisms over implementation in order to keep its structure as clear, compact, dynamic, and reconfigurable as possible. At any given moment, it should be possible to enact any creative thought about sound, image, space, and time and experiment with a range of possible approaches without having to divert energy away from its pursuit due to the brittleness of the computational system. Computation must become plastic. Software must become more soft.

LuaAV is a generative kernel of computation, capable of spawning new structures and procedures dynamically. In essence, it is meta-software for audiovisual composition capable of generating a vast range of possible softwares either at the hand of a user/programmer or even algorithmically through abstract machines. As much of LuaAV as is reasonable is generated dynamically through built-in compiler, parser, and code generating mechanisms such that the software grows and adapts as features are employed.

LuaAV is divided between a cross-platform library called libluaav and collection platform-specific classes and functions that make up the LuaAV application. libluaav contains the core timing, scheduling, and multi-threaded messaging functionality and is designed in such a way that it can be embedded in a wide variety of contexts much like Lua itself. Possible contexts might include VST plugins, web-browser plugins, streaming radio stations, or other host applications such as 3D modeling programs. In such contexts platform-specific functionality such as windowing and menus are irrelevant, which is why they are in the LuaAV application instead of libluaav.
