Architecture
============

Logical Interfaces
------------------

.. logic_arc_int:: Read JSON
   :id: logic_arc_int__baselibs__read_json
   :security: YES
   :safety: ASIL_B
   :fulfils: feat_req__baselibs__json_library
   :status: invalid

   * FromFile

.. logic_arc_int:: Write JSON
   :id: logic_arc_int__baselibs__write_json
   :security: YES
   :safety: ASIL_B
   :fulfils: feat_req__baselibs__json_library
   :status: invalid

   * FromAny
   * ToJsonAny

Components
----------

.. comp_arc_sta:: C++ Baselibs
  :id: comp_arc_sta__baselibs__cpp_baselibs
  :security: YES
  :safety: ASIL_B
  :fulfils: comp_req__baselibs__cpp_json
  :implements:
  :status: invalid
  :includes: comp_arc_sta__baselibs__cpp_json

  C++ Baselibs Module

.. comp_arc_sta:: C++ JSON lib
  :id: comp_arc_sta__baselibs__cpp_json
  :security: YES
  :safety: ASIL_B
  :fulfils: comp_req__baselibs__cpp_json
  :implements: logic_arc_int__baselibs__read_json, logic_arc_int__baselibs__write_json
  :status: invalid

  JSON library