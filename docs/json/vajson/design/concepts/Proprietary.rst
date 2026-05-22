Proprietary JSON Format
=======================

.. Concept:: DSGN-Concept-ProprietaryJson

  The JSON specification RFC-8259 is extended by Vector as a proprietary extension to allow for plain binary content and
  binary strings.
  Both types are represented as JSON values and handled accordingly (i.e., they must follow the JSON specification for
  values).

  The binary representation allows for increased parsing speed because the length of the value is known beforehand and
  the value may be (in the best case) read at once from the underlying buffer.
  Additionally, instead of writing binary values as array of ASCII numbers, they can be directly written and read.

  The length information of binary values is serialized as a four byte number in network byte order / big endian.
  The length information gets prepended to the actual value.

  For binary strings, no character escaping will be done and the string is written as is, without double quotes.
  A 's' literal is prepended to the length information.
  A resulting binary string looks like the following example (the binary zeros contained in the length information are
  printed as ASCII zeros to enhance the readability of this example):
  `{"key": s000#This is a string that has 35 chars.}`
  The read string value is passed to the OnString callback of the parser.

  Binary keys are handled in the same way as binary strings.
  A 'k' literal is prepended to the length information.
  A binary key is not followed by a colon.
  The read key is passed to the OnKey callback of the parser.

  For binary values, a 'b' literal is prepended to the length information.
  A resulting binary value looks like the following example (again, the binary zeros are ASCII zeros):
  `{"key": b000%abcdefghijklmnopqrstuvwxyzABCDEFGHIJK}`
  The read binary value is passed to the OnBinary callback of the parser.
