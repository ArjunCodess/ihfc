# Two-robot redesign for Robotics for Good 2026-2027

SealBot and CareBot | Design revision: 5 September 2026

## 1. The recommendation

Build SealBot for samples and containment, and CareBot for medical kits and patient triage. Use a dedicated Pixy2 color-tracking camera on CareBot, with a shaded inspection pocket and fixed white lighting. Keep motion control on a separate microcontroller. I would choose this over a classic ESP32-CAM because it leaves more development time for reliable pickup and delivery.

CareBot should hold patients in twelve individually addressed pockets instead of shared color bins. An uncertain patient can then be retained and rechecked without contaminating a whole bin. SealBot should collect and deliver samples before closing the perimeter, using separate sample trays and an adjustable insertion head. Neither robot needs a full articulated arm.

This is a replacement engineering concept and build specification, not tested hardware or fabrication-ready CAD. Dimensions and times are prototype targets unless attributed to the rulebook. The architecture is selected; the acceptance tests determine its final dimensions and competition task set.

## 2. Rules that determine the design

The supplied Rulebook-2026.pdf is the rule source. Design for Senior randomization; Junior mode can use fixed pickup coordinates. Confirm any event amendments before competition.

| Requirement | Design consequence and source |
|---|---|
| Robots, kits, and beams all start within 480 x 280 mm | Gauge the complete loaded assembly, including decoration and guards. Page 4. |
| The match lasts 120 seconds | Reserve delivery time and enforce an independent shutdown deadline. Pages 3 and 18. |
| Three discs, diameter 56 mm and thickness 5 mm | Use separate retaining trays rather than a stacked shutter. Pages 8-9, including the drawing. |
| Laboratory drawing: 440 x 150 x 3 mm panel, three 60 mm holes | Confirm installed orientation, hole centers, and receiving clearance. Page 9. |
| Beams: 250 and 280 mm long, section 60 x 20 mm | Release upright, stable on the narrow base, and completely unsupported. Pages 5 and 9-10. |
| Each beam touches a fixed wall, the other beam, and the marked line | Prove the corner joint and approach geometry together. Page 5. |
| Ten kits, 25 x 25 x 20 mm | Deliver six to H and two to each PCC. Pages 6 and 10. |
| Twelve patients, four per color, 20 x 20 mm | Provide one physical storage pocket per cylinder. Pages 11-12. |
| Senior patient arrangement may vary | Scan legal side areas; do not assume only color order changes. Pages 7 and 16. |
| Accessible emergency stop, safe edges, secured components, SDG 3 decoration | Include them in the initial packaging and inspection. Page 17. |

### Scoring and unresolved wording

The Senior positive scores on page 21 total 250: samples 50, containment 70, kits 50, and triage 80. This is a theoretical maximum. Both beams and the exact kit distribution account for 120 positive points, so those mechanisms deserve early testing.

The penalty wording conflicts. Page 21 gives minus three for a patient outside its correct destination; page 23 gives minus five for a patient placed in an incorrect destination and adds minus five for a sample left in quarantine. Page 7 says incomplete actions do not score, which does not resolve the later provisions. Obtain the organizer's ruling on retained and uncollected patients and the governing penalty table. An onboard reject is not assumed penalty-free.

Yellow patients may all go to one PCC without a wrong-destination penalty, according to pages 7 and 24. A two-and-two split earns the bonus. When time is short, prioritize correct delivery over the split.

## 3. Why Pixy2 rather than ESP32-CAM

| Option | Work required | Decision |
|---|---|---|
| Pixy2 | Teach signatures, filter detections, calibrate optics, integrate wired results | Recommended for this color-specific task. |
| HUSKYLENS | Teach colors, read IDs, validate focus and stability | Sensible if already owned; use its own firmware adapter. |
| Classic ESP32-CAM | Develop capture, exposure control, segmentation, rejection, and result messaging | Choose for a constrained budget or existing embedded vision experience. |
| ESP32-S3 camera board with PSRAM | Similar custom software, with board-specific integration | My preference if building a new custom ESP camera system. |
| Close-range RGB sensor | Control distance and illumination, classify aggregate readings | Adequate after capture, but cannot locate patients during approach. |

Pixy2 returns color signatures, region positions, sizes, and tracking information. These support approach alignment and inspection. They are not calibrated confidence probabilities. Firmware must check physical occupancy, region geometry, and stability before accepting a color. Its API also supports polling for new results. See S2.

HUSKYLENS provides onboard color recognition and UART/I2C communication. It can replace the chosen camera after fixture testing, but its protocol and output handling differ. See S3.

Espressif warns that RGB/YUV capture strains memory bandwidth and can lose image data, especially with Wi-Fi enabled. ESP32-CAM can still classify a stopped object, but the team must validate capture as well as color decisions. Process onboard and send results over a wire; do not depend on a laptop stream during a match. See S4.

### One camera, two observation stages

Mount Pixy2 above the front intake looking down and forward. Its view must cover an approach region ahead of the scoop and a second region inside the capture pocket. A retractable front shutter closes after capture, shading that pocket. Diffused white LEDs illuminate the cylinder from the sides without shining into the lens.

Start with an adjustable bracket roughly 80-140 mm above the floor, then choose the actual distance through focus and field-of-view tests. This is a fixture starting range, not a camera focus specification. Use matte neutral internal surfaces and keep colored decoration and kit crosses out of view.

Approach detections are pickup candidates only. Once a presence sensor confirms capture and the shutter closes, stop the robot and inspect again. Prove that one optical setting works for approach and all three colors in the pocket. If it cannot, add a separate fixed inspection camera; do not refocus during the match.

### The acceptance decision

Teach colors using every actual painted cylinder in the assembled pocket. Include different faces, worn paint, slight tilt, and expected lighting. Use empty pockets, floor markings, and kit crosses as negative examples.

Start with this acceptance rule: physical occupancy, one dominant region within the inspection area, plausible dimensions, and the same signature in at least five of seven distinct fresh frames. Reject conflicting regions. This is a tunable rule, not a measured accuracy claim. Repeated reads of one frame do not count as new evidence.

Allow one bounded re-read after settling. If still uncertain, store UNKNOWN. Never infer a color just because four of another color have been counted; inconsistent counts should trigger review.

For an ESP implementation, use low-resolution local images and mask the captured cylinder. Exclude dark and clipped pixels, handle red across the hue wraparound, and stabilize exposure and white balance under the installed LEDs where supported. Benchmark capture formats and buffering on the selected board. Send frame sequence, capture status, and classification to the motion MCU so stale results cannot release a patient.

## 4. Common chassis and loaded fit

Use two-wheel differential drive, encoders, compliant caster support, a reflectance array, obstacle sensing, and soft docking switches. Share motors, drivers, connectors, and firmware structure between robots. Keep batteries low and allow different upper decks.

Target 220 x 260 mm loaded per robot. Two robots with a 10 mm gap occupy 450 x 260 mm, leaving nominal total margins of 30 mm across and 20 mm in depth. Verify this with a rigid starting-zone gauge; bare chassis dimensions are insufficient.

Store SealBot's beams diagonally and upright. At 30 degrees to chassis length, a 280 x 20 mm footprint projects to approximately 253 mm length and 157 mm width. This leaves only about 7 mm total length for clearances and retainers. Recess end restraints within that envelope and check the actual wood and cassette in CAD. A slight unspecified angle is not a fit proof.

Use a separate motion MCU per robot with encoder inputs, a watchdog, and a wired camera interface. An RP2040-class board is a candidate after a pin budget. Select driver or I/O expansion boards where needed. Choose motors from measured loaded mass, traction, and stall current.

A 65 mm wheel at 120 rpm corresponds to about 0.41 m/s without slip. Plan lower loaded speed and roughly 0.05-0.10 m/s at pickup and docking. Turns, acceleration, settling, and mechanism time must be included in route estimates.

Use a protected battery, wiring-rated fuse, separate servo and logic regulation, a master disconnect, and a latching hardware emergency stop that cuts actuator power. Logic may remain on for diagnostics. Reset must never resume movement automatically. Test voltage during simultaneous motor and servo loading and include guards and decoration in the fit check.

## 5. SealBot: samples before closure

### Intake, metering, and storage

Use a 120-140 mm compliant intake, rounded thin lip, and slow foam roller feeding a roughly 60 mm channel. A metering gate isolates one disc. Separate entrance and seated-piece sensors confirm a completed pickup.

Store discs flat in three individual trays on a vertical indexer. Start with 59-60 mm internal width and adjustable clearance above the 5 mm disc. Each tray needs retention and a pusher opening. Confirm seating before closing the retainer and indexing another tray. One slow reverse is allowed for a jam while still in the collection area. Unconfirmed transfers do not increment the count.

Search the open quarantine area with overlapping passes and a bounded edge pass. A downward object sensor assists intake detection, but must be tested against black tape and the dark discs. Tape alone cannot confirm collection.

Two intake-width sweeps do not prove complete coverage: chassis clearance, wall contact, and funnel geometry matter. Map the reachable area with the loaded robot. Use compliant guides to draw wall-adjacent discs inward. Change intake reach if edge pickups fail; do not compensate by repeatedly pushing into walls.

Stop after three confirmed discs or the search allowance expires. Compare another attempt with the time required to deliver carried samples and place beams. Do not plan to cross the completed barrier.

### Adjustable laboratory head

Use the tray indexer as a lift, with a short tilt cradle and compliant linear pusher at its output. This accommodates horizontal or upright installation once the laboratory is measured.

For an upright panel, retain and rotate one disc into the panel's plane, lift to the measured center, then push it through. For a horizontal receiver, keep it flat and use a controlled drop guide. All guides remain attached to the robot.

The 56 mm disc has only 2 mm nominal radial clearance in a 60 mm hole. Dock compliantly against a permitted physical reference and measure each hole offset. Do not estimate spacing from the schematic. Limit insertion force and stop on overload or timeout.

An empty tray confirms release, not a scored sample. Validate full insertion and receiving clearance on the actual replica. Installed orientation, hole centers, approach clearance, and the interpretation of completely inside must be settled before fabrication release.

### Beam carriage

Carry beams in two vertically separated diagonal cassettes with supporting floors, padded jaws, and confirmed latches. A guided carriage lowers the active cassette to the field while the other beam remains retained. Mount above the sample mechanism and check both assemblies' swept volumes.

After sample delivery, align the diagonal cassette using the robot heading and approach from outside the intended enclosure. Lower the beam onto its narrow base, open the jaws, withdraw its thin support, then move away normal to the beam. Opening a latch alone cannot safely place a suspended beam.

Choose the first edge in the full-size corner fixture. Place the second with low-force compliant overtravel to close the end joint and reach the wall. Test the actual lengths, tape position, and cassette offset together. Servo position does not prove beam contact. Finish clear of both unsupported beams.

The stacked carrier raises the center of mass and is SealBot's main mechanical risk. Test loaded braking and turning. If it cannot deploy from outside the enclosure, revise the geometry before finalizing the chassis.

## 6. CareBot: controlled capture and individual storage

### Medical kits

Use destination magazines for two, six, and two kits. Each needs a holdback finger and a metering exit gate; one trapdoor cannot meter a single kit while retaining the others. Confirm each exit and record the destination count.

Release through low chutes well inside each zone. Withdraw without dragging pieces back over its boundary. If six stacked kits make the robot too tall, use two three-kit columns for the hospital. Check these around the carousel and camera.

Normally empty each assigned magazine on one destination visit. Keep a one-kit-per-destination protection route as an option if timed trials justify the extra travel under the empty-zone penalty. Do not assume both strategies cost the same time.

### Capture and lift

Use a shallow U-shaped scoop with compliant guides and an initial 23-24 mm pocket for a 20 mm patient. Tune using the painted pieces. A soft roller or pusher seats the cylinder. Presence sensing and a front shutter isolate one patient for the camera.

A tipped, partially captured, or ambiguous object gets one bounded reseating attempt. After inspection, a short guided elevator raises it to the carousel port. Use position sensing and positive retention throughout travel. This replaces the exposed sorting belt.

### Twelve-pocket carousel

Use twelve upright pockets above the drive deck. As a starting geometry, 24 mm bores on a 54 mm radius have adjacent centers about 28 mm apart. Allow roughly 140-150 mm overall diameter including walls and rim, then verify clearances and kit-magazine packaging.

A stationary support plate under the rotating pockets has one controlled transfer port. A geared index drive with position feedback, home sensing, and a detent selects pockets in 30-degree increments. Step counts alone do not confirm alignment. The port stays closed whenever the elevator is away.

Use a slotted elevator platform and matching retractable support fingers at the port. To load, align an empty pocket, raise the cylinder fully into it, extend the support fingers through the platform slots below the cylinder, then lower the platform clear. To unload, raise the platform through those gaps to take the weight, retract the fingers, and lower the cylinder. Confirm support transfer before any rotation. Prototype this geometry rather than letting a solid shutter collide with the elevator.

At a destination, select a confirmed matching patient, lower it into the front pocket, and push it gently inside the zone. Confirm departure before marking the storage pocket empty. Close the port before rotating again.

Each record holds occupancy, color or UNKNOWN, inspection status, and delivery status. A failed index blocks transfers until safe rehoming. An uncertain physical transfer stops the subsystem; inventory must not be updated optimistically.

### Route and reinspection

After kit delivery, scan the legal side areas from field references. Known stickers provide initial waypoints, while camera detections guide final approach and local search. Physical capture sensing rejects floor markings that resemble patients.

Collect up to six from one side, deliver reachable confirmed patients, then scan the other side. This allows scoring before all twelve pickups succeed. Exact waypoints and first-side choice follow measured field geometry and two-robot clearance checks.

Send red to H, green to RZ, and yellow to PCCs. Assign yellow toward the smaller delivered-plus-assigned count, aiming for two per PCC. If another trip threatens correct delivery, use the nearer PCC and lose the split bonus.

Reinspect UNKNOWN by lowering its stored cylinder into the same shaded pocket. Allow one later reinspection if time permits. Never discard it onto the field or into a guessed zone. Possible penalties follow the organizer's ruling.

## 7. Navigation and coordination

Use encoders for travel, reflectance sensing for known tape, and range or soft contact sensing for docking. Central black markings are references, not necessarily physical barriers. Keep distances, zone edges, offsets, and stopping distances configurable.

Activate both robots at the official start. CareBot departs first while SealBot executes a short stationary state. SealBot uses the lower-left sample and laboratory area. CareBot uses side pickup areas and upper destinations, with a tested return to RZ that clears SealBot's carrier and parking location.

A fixed delay cannot prevent collisions after a jam. Define shared corridors using full moving envelopes. SealBot yields outside a shared corridor to CareBot, but both stop for sensed obstacles. Persistent blockage triggers a timeout and abandonment or safe parking, rather than blind continuation. Avoid a configuration in which both robots wait inside the same corridor.

No radio is required. If added, link loss must preserve obstacle handling. Neither robot parks in the recovery delivery area or touching a scored beam.

## 8. Controller and time budget

Use nonblocking state machines with entry actions, success checks, deadlines, and failure exits. A watchdog and match deadline operate independently of camera responses. Reset during a match enters an inhibited state.

SealBot sequence: self-test, wait, exit, search and capture, dock and insert, place first beam, place second beam, park, stop.

CareBot sequence: self-test, wait, deliver kits, search, capture, inspect, store, select delivery, deliver, optionally reinspect, park, stop.

Every transfer follows preparation, alignment confirmation, movement, occupancy confirmation, and inventory commit. Permit one local recovery. Overcurrent, impossible switch states, stale vision, or lost position reference stops the affected mechanism while retention remains safe.

| Robot | Phase | Initial allowance |
|---|---|---|
| SealBot | Start, exit, and sample capture | 35 s |
| SealBot | Laboratory docking and three insertions | 30 s |
| SealBot | Two beam placements | 30 s |
| SealBot | Recovery and parking reserve | 23 s |
| CareBot | Kit distribution | 30 s |
| CareBot | Patient search, capture, inspection, storage | 42 s |
| CareBot | Deliveries including RZ return | 30 s |
| CareBot | Recovery and parking reserve | 16 s |

Each total is 118 seconds. These are unvalidated allocations. CareBot's collection target averages only 3.5 seconds per patient, including search and storage. Its delivery allowance must cover travel and twelve individual unloads. Measure complete cycles before committing to a full triage run.

After every action, compare remaining time with measured delivery time for onboard patients plus parking and reserve. Stop acquiring when that margin is exhausted. Do not begin a transfer that cannot finish before the deadline. Command actuator shutdown at 118 seconds; payload retention must remain safe without powered motion.

## 9. Procurement specification

| Assembly | Items and selection check |
|---|---|
| Each base | Two encoder motors, wheels, compliant support, driver, MCU. Size from mass and stall current. |
| Each navigation set | Reflectance array, obstacle sensing, docking switches. Validate on the actual field. |
| Each electrical set | Battery, fuse, emergency stop, master switch, regulators, wiring, guards. Test brownouts. |
| CareBot vision | One Pixy2, adjustable mount, diffuse LEDs, shutter, presence sensor. Prove both image regions. |
| CareBot patients | Scoop, capture actuator, guided elevator, carousel, index drive, position sensors, support fingers. |
| CareBot kits | Three destination magazines, holdback and metering gates, exit sensing. Hospital capacity six. |
| SealBot samples | Roller, gate, three-tray indexer, retention sensing, tilt cradle, limited-force pusher. |
| SealBot beams | Two padded cassettes, guided carriage, latches, end sensing, stable supports. |

Measure mechanism forces before choosing servos. Calculate torque at the actual lever arm, allow for friction and acceleration, and test the power rail under combined load. Component selection must be followed by a wiring diagram and pin allocation. This specification does not claim a final motor rating, battery capacity, or fabrication drawing.

## 10. Build order and acceptance

1. Build the field fixtures and loaded starting gauge. Confirm laboratory installation, legal patient variation, and penalty interpretation. Measure the actual game pieces.

2. Build the camera-and-capture fixture first. Test at least 300 labeled presentations across all cylinders, lighting changes, empty pockets, partial captures, and kit crosses. Target zero wrong accepted colors and no more than five percent UNKNOWN. Zero observed mistakes is not proof of zero future errors.

3. Prototype the carousel handoff. Run 200 mixed loads, indexes, and unloads with no inventory drift, unintended release, or wrong-pocket transfer. Inject a blocked port and missed position signal to test interlocks.

4. Prove the loaded beam carrier. Target at least 48 successful paired closures in 50 trials, with unsupported beams, correct contacts, and no damage or manual correction.

5. Test sample capture on at least 30 varied legal layouts, including edge and corner cases. Prove laboratory insertion separately before combining the route.

6. Tune one drive base and duplicate it. Validate loaded travel, tape reacquisition, turning, low-battery stopping, and clearance throughout mechanism motion.

7. Target at least 19 exact kit distributions in 20 full trials. Then add triage and compare measured pickup and unload cycles with the budget.

8. Run both robots together for at least 20 randomized matches. Include delays, a missed pickup, camera timeout, and blocked corridor. Require zero collisions, safe failure handling, and shutdown before 120 seconds. Record scores using the confirmed rules.

9. Freeze the competition task set from results. If twelve-patient triage does not fit, shorten acquisition and deliver confirmed onboard patients. Disable unreliable subsystems through configuration and recalculate expected score with penalties included.

The camera fixture, sample insertion fixture, and loaded beam cassette determine whether the packaging works. Prove them before buying or cutting two final chassis.

## 11. What changed

Camera-guided acquisition and controlled reinspection replace the point color sensor. Individual pockets and a short elevator replace color bins and the long sorting belt. Sample delivery precedes closure, separate trays replace a disc stack, and an adjustable head replaces the assumed laboratory drop funnel.

The rewrite removes the unsupported two-sweep guarantee, exact-depth starting arrangement, and penalty-free reject assumption. The carousel and beam carrier require careful development, but provide explicit retention and transfer checks. Fit, timing, and reliability are now acceptance tests rather than promised outcomes.

## 12. Sources and release measurements

- S1. Supplied Rulebook-2026.pdf. Pages 4-17 cover objects, layout, and robots; pages 20-24 cover Senior scores and penalties. Drawings on pages 9-10 and 15-16 were visually inspected.
- S2. [Pixy2 color connected components API](https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:ccc_api). Official signature, geometry, tracking, and polling reference. Accessed 5 September 2026.
- S3. [DFRobot HUSKYLENS](https://www.dfrobot.com/huskylens/huskylens). Official color-recognition and UART/I2C description. Accessed 5 September 2026.
- S4. [Espressif ESP32 camera driver](https://github.com/espressif/esp32-camera). Official processor support, memory requirements, and capture caveats. Accessed 5 September 2026.

Before fabrication release, record laboratory orientation and hole centers, beam joint geometry, patient-position variation, loaded dimensions and mass, camera working distance, actuator forces, full cycle times, and the organizer's penalty ruling. No final route coordinates or untested performance figures should be inferred from this concept.
