# Notes

## Findings

### `dependable_element_bitmanipulation` angelegt — baut, Lobster-Test war erwartbar rot, jetzt behoben (siehe Finding weiter unten)

**Kontext:** `dependable_element(name="dependable_element_bitmanipulation", assumptions_of_use=[":aous_bitmanipulation"],
requirements=["@score_platform//docs/features/baselibs/requirements:feat_req_baselibs"],
architectural_design=[":arch_design_bitmanipulation"], dependability_analysis=[], components=[":component_bitmanipulation"],
tests=[], integrity_level="B")` in `score/bitmanipulation/BUILD` angelegt.

**Zwei neue, bisher unbekannte Stolperfallen bis der Build lief:**
1. **`unit().scope` braucht absolute Labels, keine relativen.** `dependable_element`s "Certified
   Scope"-Check verlangt, dass jedes `unit()`, dessen `implementation`-Target selbst (nicht nur
   dessen transitive Abhängigkeiten!) im Scope steht — sonst Fehler "Not in certified scope
   @@//score/bitmanipulation:bitmanipulation, stopping at score". `scope = [":bitmanipulation"]`
   (relatives Label) schlägt fehl, weil `Label(str)` in `dependable_element.bzl` relative Strings
   relativ zum **Paket der .bzl-Datei selbst** auflöst, nicht relativ zur aufrufenden BUILD-Datei.
   Fix: immer volle Labels verwenden, z. B. `scope = ["//score/bitmanipulation:bitmanipulation"]`
   (siehe auch die Beispiele in `tooling/.../examples/seooc/unit_1/BUILD`, die dasselbe Muster
   zeigen).
2. **PlantUML-Paket-Alias muss exakt dem `dependable_element`-Target-Namen entsprechen.** Die
   `validation_cli`-Architekturprüfung verlangt ein Top-Level-`package "..." as <dependable_element
   name> <<SEooC>>` im `architectural_design`-PlantUML. Unser `static_design.puml` hatte
   `as bitmanipulation`, musste zu `as dependable_element_bitmanipulation` umbenannt werden (Alias,
   nicht der Anzeigename).
3. **Systemabhängigkeit fehlte:** Der allererste Build eines `dependable_element`-Targets in diesem
   Workspace hat eine neue Pip-Wheel-Auflösung (`pycairo`, für den Sphinx/Graphviz-Doku-Stack)
   ausgelöst, die ohne `pkg-config`/`libcairo2-dev` fehlschlägt ("Dependency lookup for cairo ...
   failed"). Einmalig behoben via `apt-get install -y pkg-config libcairo2-dev` (root im Container,
   kein Workspace-Zustand).

**`bazel build //score/bitmanipulation:dependable_element_bitmanipulation`: SUCCESS** (HTML-Doku +
Lobster-Report werden erzeugt).

**`bazel test //score/bitmanipulation:dependable_element_bitmanipulation`: war zunächst FAIL** — 13
von 14 `feat_req_baselibs`-Einträgen bekamen "lobster error: missing reference to Component
Requirements". Das war die direkte Konsequenz der ursprünglich pragmatischen Entscheidung weiter
unten (`feat_req_baselibs` komplett statt eines schlanken `feat_req_bitmanipulation_only`): nur
`feat_req__baselibs__bitmanipulation` hatte ein zurückverweisendes `comp_req`; die anderen 13
(unrelated) Feature Requirements des `baselibs`-Moduls hatten keins und ließen den
Lobster-Traceability-Test am `dependable_element`-Level (Feature Req ↔ Component Req)
fehlschlagen. **GELÖST** — siehe Finding "dependable_element.requirements zieht immer die komplette
RST-Quelldatei mit" weiter unten: die zuvor verworfene Alternative (schlankes,
bitmanipulation-only Feature-Requirements-Target) wurde doch noch umgesetzt.

### `assumptions_of_use()` für `bitmanipulation`: gleiche `index.rst` wie `comp_req`, dank `only_types`-Patch

**Kontext:** `docs/requirements/index.rst` (bitmanipulation) enthält bereits sowohl die
`comp_req`- als auch die `aou_req`-Direktiven (Abschnitt "Assumptions of Use (AoU)"). Ziel: diese
per `assumptions_of_use(name="aous_bitmanipulation", srcs=["docs/requirements/index.rst"],
requirements=[":comp_req_bitmanipulation"])` verwenden — **ohne** eine zweite/duplizierte RST-Datei
anzulegen (AoU-Ebene bewusst auf **Component**, nicht Feature: passt zur eclipse-score-Konvention
"AoUs zählen zu Component Requirements", siehe `reference_integration`-Repo).

**Zwei Probleme, beide durch kleine, gezielte Patches im lokalen `score_tooling`-Fork gelöst:**
1. `rst_to_trlc`s `declare_file(..., sibling = src)` erzeugte für zwei Rules (`component_requirements`
   und `assumptions_of_use`), die dieselbe `.rst`-Datei referenzieren, denselben Output-Pfad →
   "conflicting actions". Fix: Output wird jetzt unter `ctx.label.name` (statt `sibling = src`)
   deklariert (`rst_to_trlc.bzl`), wodurch jede Rule ihren eigenen, eindeutigen Output-Pfad bekommt.
2. `parse_directives()`/`render_trlc()` konvertierten bislang **alle** in `DIRECTIVE_TO_TRLC`
   bekannten Direktiven einer Datei, unabhängig davon, welche Rule sie aufgerufen hat — d.h.
   `assumptions_of_use()` hätte versehentlich auch die `comp_req`-Einträge (samt ungelöstem
   `derived_from` → `TODO_PACKAGE`-Fehler) mit in sein TRLC-Package gezogen, und umgekehrt. Fix:
   neuer optionaler `only_types`-Parameter (durchgereicht via CLI `--only-types` in
   `rst_to_trlc.py`, Attribut in der `rst_to_trlc`-Rule, `rst_srcs_to_trlc()`-Helper). Anhand von
   `req_kind` (`"assumed_system"`, `"feature"`, `"component"`) wird in `requirements.bzl`s neuer
   `_REQ_KIND_TO_DIRECTIVES`-Map automatisch die passende Direktivenmenge gewählt (`"assumed_system"`
   akzeptiert sowohl `assumed_system_req` als auch `stkh_req`, siehe Finding weiter unten);
   `assumptions_of_use()` verwendet fest `only_types=["aou_req"]`.

**Verifiziert:** `bazel build //score/bitmanipulation:aous_bitmanipulation
//score/bitmanipulation:comp_req_bitmanipulation //score/bitmanipulation:component_bitmanipulation`
läuft durch; generiertes `.trlc` von `aous_bitmanipulation` enthält exakt die 3 `ScoreReq.AoU`-Records,
das von `comp_req_bitmanipulation` exakt die 5 `ScoreReq.CompReq`-Records (keine Vermischung mehr).
`bazel run //:docs_check`/`//:live_preview` (baselibs) laufen fehlerfrei durch.

**Nebenbei behoben:** `comp_arc_sta__baselibs__bitmanipulation_component` (ID aus der früheren
`comp_arc_sta`-Architekturarbeit) überschritt mit 49 Zeichen das von `score_metamodel` erzwungene
45-Zeichen-Limit für `.id`-Felder — das war die eigentliche Ursache für den `Exit Code 3` bei
`bazel run //:live_preview`, unabhängig von der AoU-Arbeit. Umbenannt zu
`comp_arc_sta__baselibs__bitmanip_static` (39 Zeichen).

### ~~`assumed_system_req` bricht `bazel build //:docs` / `//:live_preview`~~ — GELÖST

**Ursprüngliches Problem:** Um `assumed_system_requirements()` (neue Rule im lokalen
`score_tooling`-Fork) zu testen, wurde in `score_platform` (`../score`) eine **neu erfundene**
RST-Datei `assumed_system_requirements.rst` mit einer **neuen, selbst ausgedachten**
sphinx-needs-Direktive `.. assumed_system_req::` angelegt und zusätzlich per `docs_bundle` in die
normale HTML-Doku eingespeist. Das brach `bazel build //:docs` / `//:live_preview` mit
`ERROR: Unknown directive type "assumed_system_req"`, weil `score_docs_as_code`s (Registry-Version)
`metamodel.yaml` diesen Typ nicht kennt (`needs_types`: nur `std_req, gd_req, stkh_req, feat_req,
comp_req, tool_req, aou_req`).

**Fehler in der ursprünglichen Analyse:** Das war der falsche Ansatz — es gibt bereits einen
validen, registrierten sphinx-needs-Typ für genau diesen Zweck: `stkh_req` (Stakeholder
Requirements, `score/docs/requirements/stakeholder/index.rst`). Es musste also **kein** neuer
Direktiv-Typ erfunden und **kein** `score_docs_as_code`/Metamodel-Patch vorgenommen werden.

**Tatsächliche Lösung (minimal-invasiv, nur in unserem `score_tooling`-Fork):**
1. In `tooling/bazel/rules/rules_score/src/rst_to_trlc.py`s `DIRECTIVE_TO_TRLC`-Mapping einen
   zusätzlichen Eintrag ergänzt: `"stkh_req": "ScoreReq.AssumedSystemReq"`. Die
   RST→TRLC-Konvertierung schaut nur nach, welche Direktiven-Namen (unabhängig von der
   aufrufenden Rule) in der übergebenen Quelldatei vorkommen — es gibt keine req_kind-spezifische
   Einschränkung.
2. `assumed_system_requirements()` zeigt jetzt direkt auf die echte, unveränderte
   `docs/requirements/stakeholder/index.rst` (kein Duplikat/Fake-RST mehr nötig). Da
   `ctx.actions.declare_file(..., sibling = src)` verlangt, dass Output-Datei und Quelle im
   selben Bazel-Package liegen, musste dafür ein neues, eigenes `BUILD` in
   `score/docs/requirements/stakeholder/` angelegt werden (Target `asr_req_stakeholder`,
   `package = "StakeholderRequirements"`), analog zum bereits bestehenden Muster bei
   `feat_req_baselibs` (eigenes `BUILD` + `docs_bundle`-Rückmount in `//BUILD`s `bundles`, damit
   das Toctree weiter auflöst).
3. `feat_req_baselibs`s `deps`/`ref_package` zeigen jetzt auf `asr_req_stakeholder` /
   `"StakeholderRequirements"`; die erfundene `assumed_system_requirements.rst` wurde gelöscht;
   `derived_from` in `docs/features/baselibs/requirements/index.rst` wieder auf die
   ursprünglichen, echten `stkh_req__...`-IDs zurückgesetzt (kein Umbenennen mehr auf der
   RST-Seite — das "Umtaufen" zu `AssumedSystemReq` passiert jetzt ausschließlich in Schritt 1,
   also erst beim TRLC-Export).

**Tradeoff (bewusst akzeptiert, analog zum `feat_req_baselibs`-Finding weiter unten):** Die
Stakeholder-Datei enthält 97 `stkh_req`-Einträge insgesamt, aber nur 4 eindeutige IDs werden von
`feat_req_baselibs` tatsächlich referenziert — es werden trotzdem alle 97 zu
`AssumedSystemReq`-TRLC-Records konvertiert (ungenutzter, aber harmloser Ballast).

**Verifiziert:** `bazel build //docs/requirements/stakeholder:asr_req_stakeholder`,
`//docs/features/baselibs/requirements:feat_req_baselibs`, `//:docs` und `LC_ALL=C.UTF-8 bazel run
//:docs_check` laufen alle fehlerfrei durch (`build succeeded`, 0 Schema-Warnungen).

### dependable_element.requirements zieht immer die komplette RST-Quelldatei mit — GELÖST

**Kontext:** Aufbau eines `dependable_element` für `bitmanipulation` als eigenständiges SEooC
(`components = [":component_bitmanipulation"]`, `integrity_level = "B"`).

**Beobachtung:** Das `requirements`-Attribut von `dependable_element` verlangt Targets mit
`FeatureRequirementsInfo` oder `AssumedSystemRequirementsInfo` (siehe
`providers = [[FeatureRequirementsInfo], [AssumedSystemRequirementsInfo]]` in
`dependable_element.bzl`). `comp_req_bitmanipulation` (nur `ComponentRequirementsInfo`) passt hier
nicht rein – Component Requirements kommen indirekt über `components` mit rein (das
`component()`-Target kennt sie bereits über sein eigenes `requirements`-Attribut).

**Problem:** `_process_artifact_files()` (in `dependable_element.bzl`) symlinkt die komplette
RST-Quelldatei eines `requirements`-Targets 1:1 in die generierte Doku (kein Filtern einzelner
Requirements innerhalb einer Datei möglich). `feat_req_baselibs` enthält aber **alle 14
Feature Requirements** von `baselibs` (u. a. `json_library`, `flatbuffers_library`,
`static_reflection_library`, …), nicht nur `feat_req__baselibs__bitmanipulation`. Das führte auch
zum Lobster-Testfehler oben: die 13 fachfremden Requirements haben kein zurückverweisendes
`comp_req` und lassen den Traceability-Test fehlschlagen.

**Ursprüngliche Entscheidung (2026-08-26, inzwischen überholt):** Zunächst pragmatisch
`feat_req_baselibs` unverändert als `requirements`-Target verwendet, Test-Fail als bekannter Mangel
akzeptiert. Verworfene Alternative war ein separates, schlankes `feat_req_bitmanipulation_only`-Target.

**Tatsächlich umgesetzte Lösung:** Die verworfene Alternative doch umgesetzt, indem die
`feat_req__baselibs__bitmanipulation`-Direktive aus `score/docs/features/baselibs/requirements/index.rst`
in eine eigene Datei `bitmanipulation.rst` (gleiches Verzeichnis) extrahiert und in `index.rst` per
`.. include:: bitmanipulation.rst` wieder eingebunden wurde (Doku/Toctree bleibt unverändert
gerendert). `rst_to_trlc.py` ist ein reiner Regex/Zeilen-Parser ohne Sphinx-Include-Auflösung — ein
`.. include::` in der Rohtextdatei ist für ihn unsichtbar, d. h. es gibt **kein** Duplizierungsrisiko
zwischen den beiden `.rst`-Dateien auf TRLC-Ebene, solange jede Datei nur dort explizit als `srcs`
gelistet wird, wo ihr Inhalt tatsächlich konvertiert werden soll.

Zwei neue Bazel-Targets in `score/docs/features/baselibs/requirements/BUILD`:
- `feat_req_baselibs`: `srcs = ["index.rst", "bitmanipulation.rst"]` (weiterhin alle 14 Requirements,
  jetzt aus zwei Quelldateien statt einer — `rst_srcs_to_trlc()` konvertiert jede `.rst`-Datei in
  `srcs` unabhängig und mergt die Ergebnisse, das war schon vorher so und brauchte keine
  Code-Änderung im `tooling`-Repo).
- `feat_req_bitmanipulation` (neu): `srcs = ["bitmanipulation.rst"]`, liefert nur das eine relevante
  Feature Requirement, TRLC-Package-Name default-abgeleitet vom Dateinamen-Stem: `Bitmanipulation`.

In `baselibs/score/bitmanipulation/BUILD` verweisen jetzt `comp_req_bitmanipulation` (`deps` +
`ref_package="Bitmanipulation"`), `component_bitmanipulation` (`requirements`-Liste) und
`dependable_element_bitmanipulation` (`requirements`) auf `feat_req_bitmanipulation` statt
`feat_req_baselibs` — vollständige Entkopplung von den 13 fachfremden Requirements.

**Zusätzlich nötiger Fix in `score/docs/conf.py`:** Ohne weitere Maßnahme behandelte Sphinx
`bitmanipulation.rst` zusätzlich als eigenständiges Dokument (weil es im gemounteten `docs`-Baum
liegt), was zu `needs.duplicate_id` (Need doppelt registriert: einmal via `include`, einmal als
eigenes Dokument) und `toc.not_included` (Orphan-Warnung) führte. Behoben durch
`exclude_patterns = ["features/baselibs/requirements/bitmanipulation.rst"]` in `conf.py` — die Datei
wird dadurch nur noch über das `include` gerendert, nie als eigenständiges Sphinx-Dokument.

**Verifiziert:** `bazel build` + `bazel test //score/bitmanipulation:dependable_element_bitmanipulation`
(1/1 PASSED, keine Lobster-Fehler mehr) und `LC_ALL=C.UTF-8 bazel run //:docs_check` (baselibs) laufen
beide fehlerfrei durch.

### Lobster-Traceability: Unit Tests sind aktuell nicht mit Requirements verknüpft

**Komponente:** `//score/bitmanipulation:component_bitmanipulation`

**Beobachtung:** Im generierten Lobster-Report (`bazel-bin/score/bitmanipulation/component_bitmanipulation_report.json`)
gibt es 4 Traceability-Ebenen: `Feature Requirements` → `Component Requirements` → `Architecture` → `Unit Test`.
Die Kette wird aber nur teilweise geprüft:

- **`Architecture ↔ Component Requirements`** wird von Lobster tatsächlich erzwungen
  (`needs_tracing_up`/`needs_tracing_down` in der Policy des Reports).
- **`Unit Test`** ist dagegen komplett von der Prüfung ausgenommen
  (`needs_tracing_up: false`, `needs_tracing_down: false` für das Level "Unit Test").
- Keiner der 43 GoogleTest-Testfälle in `bit_manipulation_test.cpp` / `bitmask_operators_test.cpp`
  deklariert eine Verknüpfung zu einem Requirement. Das zugehörige `.lobster`-Item
  (`component_bitmanipulation.lobster`) hat für jeden Test kein `refs`-Feld.

**Konsequenz:** Aktuell ist **keine einzige Component- oder Feature-Requirement-Anforderung
tatsächlich durch einen Unit-Test nachweisbar abgedeckt** – der Build/Test ist trotzdem grün,
weil diese Verknüpfung von der aktuellen Lobster-Policy nicht verlangt wird.

**Wie es (laut Tooling) eigentlich vorgesehen ist:** `lobster-gtest` unterstützt
`::testing::Test::RecordProperty("lobster-tracing", "<tag>")` innerhalb eines Testfalls, um
ihn explizit einem Requirement-Tag zuzuordnen. Beispiel aus dem `score_tooling`-Fixture
(`tooling/bazel/rules/rules_score/test/fixtures/test_binary_unit_test.cc`):

```cpp
TEST(BinaryUnitTest, MockFunction1ReturnsExpectedValue) {
    ::testing::Test::RecordProperty("lobster-tracing", "TestComponent.REQ_COMP_TEST_001");
    EXPECT_EQ(mock_function_1(), 42);
}
```

Für `bitmanipulation` wäre das analog z. B.:

```cpp
TEST(SetBit, WithUInt8) {
    ::testing::Test::RecordProperty("lobster-tracing",
        "CompReqBitmanipulation.comp_req__bitmanipulation__bit_operations");
    ...
}
```

**Offene Punkte / mögliche nächste Schritte:**
1. Testfälle in `bitmanipulation` mit `RecordProperty("lobster-tracing", ...)` versehen,
   um eine echte Abdeckung zu deklarieren.
2. Klären, ob/wie die Lobster-Policy (`needs_tracing_up`/`needs_tracing_down` für "Unit Test")
   angepasst werden kann/soll, damit fehlende Testabdeckung tatsächlich einen Build-Fehler auslöst
   (aktuell rein informativ).

*(Quelle: Analyse von `component_bitmanipulation_report.json`, `comp_req_bitmanipulation.lobster`,
`component_bitmanipulation_architecture.lobster`, `component_bitmanipulation.lobster` und
`arch_to_reqs_from_lobster.py` im lokalen `score_tooling`-Checkout, Stand 2026-08-26.)*
