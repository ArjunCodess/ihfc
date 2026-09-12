# Two robots for the complete challenge
SealBot and CareBot | 6 September 2026 | Rs 9,910 including reserve | Rs 10,000 maximum

## 1. The design decision

Build two robots with different jobs. SealBot handles all three samples and both containment beams. CareBot carries all ten kits and collects all twelve patients in a numbered rotary magazine, then releases each patient at the correct destination. Each robot has its own ESP32-CAM and Arduino Nano. The camera finds and inspects objects; the Nano controls motion and mechanisms.

This revision includes every scored task, including the three laboratory slots, the closed quarantine perimeter, the 6/2/2 kit distribution and the 2/2 yellow-patient split. The positive scoring ceiling is 250 points. That is the mission coverage of the design, not a predicted competition score.

The extra money buys a second camera, a patient magazine, sample handling, stronger lift servos and fabrication allowance. It does not buy a guarantee that an untested school build will finish everything in 120 seconds. The magazine and laboratory placement are the two hardest parts. Build those on the bench first; the nine-day schedule depends on proving them early.

Design basis: Senior mission difficulty for this Class 11 build; one competition half; a laptop, hand tools, multimeter and full-size flat practice surface borrowed from school; parts available by the end of day 2. The rulebook's Senior eligibility is by birth year, 2009-2012. All dimensions described as targets are proposed fabrication dimensions, not dimensions supplied by the organiser. This document is a build specification, not tested firmware or production-ready CAD.

## 2. What the rulebook actually requires

The supplied Rulebook-2026.pdf was reread alongside its extracted text. All 24 page images were inspected, including the piece drawings on pages 9-12 and the field drawings on pages 13-16. Printed dimension labels take precedence over the apparent scale of illustrations, as page 8 states.

| Task | Required final state and assigned mechanism | Positive points |
|---|---|---|
| Three samples | SealBot places one 56 mm disc completely inside each of the three 60 mm laboratory slots. | 45 + 5 = 50 |
| Two beams | SealBot releases both beams upright, touching the required walls, each other and the boundary tape. | 50 + 20 = 70 |
| Ten kits | CareBot empties preloaded batches: six at H, two at left PCC, two at right PCC. | 30 + 20 = 50 |
| Twelve patients | Four red to H; four green to RZ; two yellow to each PCC. | 60 + 6 + 8 + 6 = 80 |
| Complete positive score | All four rows above. Penalties are separate. | 250 |

Page 4 allows initial beams and kits on the floor or loaded into a robot. Both robots and all pieces must fit wholly inside 480 x 280 mm. The design uses floor-staged beams and robot-loaded kits, with no manual handover during the round. Both controllers begin executing on the referee's start; programmed waiting still counts as active under page 17.

Samples are wooden discs, diameter 56 mm and thickness 5 mm. Patients are diameter 20 mm and height 20 mm. Kits are 25 x 25 x 20 mm. The two beams are 250 x 60 x 20 mm and 280 x 60 x 20 mm, standing on their 20 mm bases. Do not build the mechanisms around 25 mm cubic kits or thick sample pucks.

The laboratory drawing gives a 440 x 150 x 3 mm panel and three 60 mm holes. It does not dimension hole-centre spacing or installation height. The field illustrations show it in the lower laboratory area; the separate perspective drawing does not by itself establish its mounting orientation.

The field is 1143 mm wide and 1181 mm deep per competition half, subject to the rulebook tolerances. With healthcare at the top, quarantine is lower left, laboratory lower centre and start/RZ lower right. The 20 mm black tape is a navigation reference, not a raised wall. The central H-shaped marking is also tape, so the robots can cross it.

### Decisions made from the rulebook

1. Use a horizontal laboratory panel as depicted in the field views on pages 13 and 15. This is an inference from the assembled-field illustrations; the isolated perspective on page 9 is a part drawing. Find the three hole centres with the camera and calibrate their positions on the practice panel rather than inventing undimensioned centre spacing.
2. Use the twelve sticker sites as the patient search locations. Page 6 explicitly says each cylinder starts on a sticker, and page 16 dimensions the two rows. Detect the colour anew in each Senior round. Local visual search corrects placement error; there is no basis for assuming patients can start anywhere on the board.
3. Use the stricter listed Senior penalties when choosing risky actions. Page 21 gives -3 for a patient outside its correct destination; page 23 gives -5 for an incorrect destination and adds -5 for a sample left in quarantine. Preserve both score calculations in practice logs because the text contains both.
4. Treat the beam geometry as an unresolved inconsistency in the source, not a missing robot feature. The enlarged page-15 drawing puts the 280 mm dimension at the inner tape edges. A right-angle joint made with 250 mm length plus 20 mm thickness reaches only 270 mm. In the ordinary square arrangement, it is 10 mm short of that tape edge. No clamp or extra motor fixes the supplied geometry, and the robot must not modify the pieces.

The complete mechanism is designed below without waiting for additional information. It can build the closed beam pair, but compliance with every beam/tape condition cannot be certified from these contradictory dimensions alone. This is the one source-level limit on an otherwise complete mission plan.

![Beam and tape dimension conflict in the supplied drawing](tmp/pdfs/full-design/beam-dimension-check.png)

## 3. Robot layout and legal starting arrangement

Use two inexpensive differential-drive chassis with two geared motors, two wheels and a caster. Cut replacement upper plates from light sheet material as needed. Keep the batteries low. Target an overall stowed envelope of at most 220 mm wide by 200 mm deep per robot, including wheels, bumpers, folded scoop, wires and decorations.

Place the robots side by side, facing out of start. Behind them, stage the two beams upright and parallel in a 300 x 60 mm strip. That strip accommodates two 20 mm beam bases with a 20 mm space between them. Leave 10 mm between the robots and between the robot row and beam strip. The nominal combined envelope is 450 x 270 mm, leaving 30 mm width and 10 mm depth inside the legal box.

![Starting envelope, proposed dimensions](tmp/pdfs/full-design/start-layout.png)

The floor beams have no unattached team-made rack. They stand on their own bases; any alignment jig is removed before the referee validates the start. Both robots leave forward, so neither crosses the beams. SealBot returns to collect them after completing the samples. CareBot carries the kits on an upper rear deck and starts with its patient magazine empty.

The sample scoop folds within SealBot's stowed envelope. Its deployed reach may extend beyond 200 mm after the start, but its swept outline must be used for routes. CareBot's 160 mm magazine sits inside its wheel envelope, with a narrow lifting cup at the front and kit chutes behind or above it. Do not buy a chassis on the assumption that its advertised plate size includes wheels.

Cut a physical 480 x 280 mm inspection frame on day 1. Load the real beams, ten dummy kits and both mock-up robots inside it. Check the initial exit and SealBot's later approach to each beam. A rectangle diagram proves the space allocation only; the loaded mock-up proves that the mechanisms actually fit.

## 4. SealBot: samples before closure

SealBot works in the lower field. It has a front sample cup, a camera facing the floor ahead, and a rear low-level beam clamp. The sample cup uses one MG90S positional servo for a short lift and one SG90 for a sliding floor. One more SG90 opens and closes the beam clamp. The camera has a manually adjustable, rigid mount; it must see both sample acquisition and the laboratory release area at their working poses.

### Collect one sample deliberately

Build a shallow cup approximately 60 mm internally wide, with a 65-70 mm entrance and a smooth, thin PET scoop tongue. Trial a 0.3-0.5 mm tongue with a rounded leading edge. The closed floor is almost level with the board during pickup. A roof with approximately 6-7 mm internal clearance prevents two 5 mm discs from stacking; a light inward-opening PET flap retains the disc when the robot reverses. These clearances need testing with painted official-size pieces.

The lift is a short parallelogram linkage so the cup remains horizontal. It needs only enough travel to clear the laboratory panel and retain the disc in transport. A spring or rubber band counterbalances the empty cup, and physical stops carry the end-position loads. The servo must not continuously stall against a stop.

Use camera blob detection to locate a sample within the known quarantine area, approach slowly, centre the entrance and drive the tongue under it. Ignore the printed biohazard symbol inside the dark disc; fit the outer silhouette. Reject long tape-like shapes and candidates outside the zone. Verify a full disc in the cup before lifting. Do not count a visual target as a successful pickup.

Three discs can touch or sit near the two walls. The robot must approach from the open north or east side and may enter quarantine before closure. Test both accessible approach directions and diagonal corner approaches. A wide blind sweep is not a coverage proof: it can push a disc into a corner or leave it beside a wheel. If a target disappears, back away along the known approach and rescan before moving elsewhere.

### Place the samples into three distinct slots

For a horizontal laboratory panel, raise the cup just above it, centre the retained disc over an empty hole, stop, and retract the cup floor sideways. The disc drops only a few millimetres and remains horizontal. The sliding floor stroke must clear the full disc; budget approximately 65 mm of travel through a crank or linkage, with the mechanism contained inside the working envelope. Reclose only after confirming that the cup is empty.

The nominal radial clearance is only 2 mm: half of 60 minus 56. Wheel encoders alone will not reliably provide that. Reacquire the panel edge and hole with the camera at close range; use small forward/backward corrections and short turn-drive-turn corrections for lateral error. A differential-drive robot cannot slide sideways. Target less than 1 mm centring error on the bench, leaving the remaining clearance for piece variation and drop motion.

Measure the three actual centres and save them as field configuration. Do not infer them from pixel spacing in page 9. Use the bottom field wall as a gentle mechanical reference only where the robot can reach it without contacting the panel or a delivered sample. The camera should inspect the hole before loading the next disc, so software cannot repeatedly use the same slot.

The carried disc can hide the hole. Observe its centre with the cup lifted clear, then move the cup along its calibrated linkage path into the release position while the chassis stays stopped. Include linkage play in the centring-error measurement. Do not pretend the camera can see a hole through an opaque disc.

Maintain three slot states: EMPTY, OCCUPIED and UNCERTAIN. Only mark OCCUPIED after the cup clears and the disc is visibly seated. For an uncertain placement, inspect again and use one small correction if the geometry permits. Do not drag the cup across an already delivered sample. A disc merely sitting somewhere in the laboratory area is not a completed slot.

The chosen build follows the horizontal panel shown in the field illustrations. Do not add a rotating delivery arm for the isolated perspective drawing; that would add cost and alignment work without support from the assembled-field view. Treat complete placement as the disc footprint lying within its slot; the 5 mm disc is thicker than the 3 mm panel, so flush seating below the panel's top is not a reasonable implied requirement.

### Place the beams without lifting them

After all three samples are delivered, return to start. The rear clamp grips one upright beam near its base using two linked fingers and padded vertical contact faces. The beam stays on the board while being towed or pushed slowly. Keep the main force application around 5-10 mm above the floor to reduce tipping, and use a broad rear guide face to prevent yaw. The clamp opens clear of both beam faces before the robot withdraws.

The two staged beams need separate approach poses. Pick the nearer one without touching the second, place it, then return for the remaining beam. During transport, include the full 280 mm beam width in obstacle clearance; the chassis width is not the loaded width. Do not spin in place beside a wall with a beam attached.

At quarantine, keep the robot outside the future enclosure. Approach each final edge from its outside, bring a beam end to the relevant fixed wall at low speed, align the long face with the measured tape position, then release. Withdraw normal to the beam face so no finger catches it. Place the second beam to meet the first and its fixed wall; check the joint and tape overlap before parking. The robot never drives through the closed boundary.

Use a full-size beam jig made from practice pieces to establish which beam belongs on which edge. A valid geometry must satisfy all four constraints together: contact with its wall, contact with the other beam, tape contact and independent upright stability. A nominal closed arrangement is the 280 mm beam at x = 250-270 mm from the left wall, extending 280 mm from the bottom wall, with the 250 mm beam running from the left wall to its side at 260-280 mm from the bottom. This closes the pair but leaves the vertical beam 10 mm inside the drawn tape edge. Store beam target poses separately from the mechanism code so the actual field can be calibrated without rebuilding the robot; do not represent this nominal arrangement as fully rule-compliant.

Park outside quarantine and the laboratory, with every part of the clamp visibly clear of both beams. A beam supported by a robot at the buzzer is not valid under page 5.

## 5. CareBot: collect once, deliver by destination

CareBot uses a twelve-pocket rotary magazine instead of twelve separate return trips. Each pocket holds one patient, and software remembers the colour in that numbered pocket. Sorting is therefore a decision about which pocket to open at each destination. There is no branching colour chute and no need to eject an unknown patient into the wrong area.

### A magazine students can make

Start with a 160 mm outside-diameter rotor. Place twelve 24-25 mm internal-diameter pockets at 30-degree intervals on a 120 mm pitch circle. Adjacent centres are approximately 31.1 mm apart, leaving material between the pockets. Short smooth tubes or accurately cut spacers form walls about 23-25 mm high. A fixed, low-friction floor supports all patients from underneath.

![Patient magazine and transfer sequence](tmp/pdfs/full-design/patient-magazine.png)

Use a central bolt with spacers and a washer or small bearing to carry the rotor weight. The 28BYJ-48 stepper turns a coupling; its shaft does not carry the loaded magazine. Keep the rotor parallel to the floor and the underside gap below 1 mm so a patient's bottom edge cannot wedge between the moving rotor and the fixed floor. A fixed lid over the stored pockets prevents pieces bouncing out, with an opening only at the loading station.

At the front station, the fixed floor has one outlet, normally covered by an SG90-operated sliding gate. Above it, the lift cup transfers a patient into the aligned pocket while this outlet gate remains closed. Below it, a short smooth chute reaches close to the board, forward of the wheels. During unloading the lift cup stays out of the path. The same indexed station is used for loading and unloading, which avoids a second alignment mechanism.

Drive the rotor through a ULN2003 board. Put twelve position marks or notches on the rotor, one distinguishable as home, and read them with an optical interrupter. Count actual arrivals at each station rather than assuming an exact gearbox ratio. Always approach final positions in the same rotational direction to take up backlash. The target is less than 1 mm port misalignment; if the loaded rotor cannot meet that repeatedly, fix the bearing and index system before writing the full mission.

A printed full-size template helps position the pockets, but it is not a substitute for drilling them accurately. Use a school drill press, hole saw or locally cut plates. The budget includes a small fabrication allowance. Freehand cardboard pockets that rub or change shape under load will cost more match time than this mechanism saves.

### Lift and inspect each patient

The front cup has a 40-45 mm mouth narrowing to a 24-25 mm pocket, a thin rounded scoop tongue, and a light retaining flap. Its short four-bar linkage keeps the patient upright while an MG90S lifts it to the magazine inlet. An SG90 slides the cup floor open to transfer it into an empty magazine pocket. The lift cup must reach the inlet with its floor directly above the pocket; test this with a cardboard side profile before drilling linkage pivots.

A presence switch or optical flag confirms capture. Lower the camera to a repeatable inspection angle, illuminate the cup with diffused white LEDs and read the actual painted patient. Then raise the camera to its acquisition angle to find the next patient. One SG90 provides these two camera positions; add physical stops so the camera view is repeatable.

Transfer only after the outlet gate is confirmed closed and the intended magazine pocket is empty. Verify that the cup became empty and the pocket became occupied before committing its colour to memory. A failed transfer leaves the inventory uncertain and stops indexing. Otherwise one missed drop could shift every later patient assignment.

Store UNKNOWN as a real state if the colour is unclear. Keep that patient in its own pocket and re-inspect it at the station before delivery. Do not decide it is green simply because it failed the red and yellow tests, and do not overwrite an occupied pocket after twelve acquisitions.

### Deliver all kits with three gates

Preload six hospital kits in one covered single-file chute and two kits in each of two smaller PCC chutes. A straight six-piece lane needs at least 150 mm of usable length, plus clearance and its gate. Trial an internal width around 28-30 mm and a modest downward slope, increasing it only if the actual wooden pieces do not slide reliably. Keep the lane exits beyond the chassis and away from the patient outlet.

Each chute has its own SG90 gate and empties its whole pre-counted batch at one destination. There is no timed individual-kit metering. Use a light spring to bias each retention gate closed, with the servo opening against it; do not depend on continuous servo torque to hold a load. Aim the outlet well inside the zone, open while stationary, inspect for retained kits and close before leaving. Load the 25 mm kit dimension along the chute for the conservative length calculation.

Keep kit drop locations and patient drop locations distinct within each healthcare zone so one delivery does not scatter the other. The 180 mm zone depth shown on page 14 makes outlet reach important; the entire robot need not drive into the zone, but each released piece must finish inside it.

### Deliver patients without losing the yellow split

After collecting twelve confirmed patients, visit left PCC, H and right PCC across the top boundary, delivering the relevant kit batch and patients at each stop. Select any two confirmed yellow pockets for the first PCC and the remaining two for the second. This allocation is independent of which side the patients were collected from.

At a stop, advance the rotor in its normal direction through the relevant pockets. Open the outlet only for a matching confirmed patient, close it, and verify the pocket is empty before continuing. The chute terminates close to the board and points into the zone to reduce rolling. At H release all four red patients. Return down the right side to start/RZ and release all four green patients after verifying the area is clear.

An empty-pocket observation confirms departure from the magazine, not destination correctness. The robot must also inspect the landing area or use a tested short chute and a large boundary margin. Delivery counts become final only after the piece clears the robot and is within the correct zone. End with all gates closed and the robot clear of delivered pieces.

## 6. ESP32-CAM perception that fits the hardware

Use two AI-Thinker-compatible ESP32-CAM boards with OV2640 cameras and working PSRAM. CareBot's camera performs patient acquisition and controlled colour inspection. SealBot's camera finds samples and aligns with laboratory holes. A camera replaces neither wheel feedback nor mechanical retention.

Use Espressif's camera driver at QVGA JPEG with one frame buffer, then decode locally for a small region of interest. A full 320 x 240 RGB888 image occupies 230,400 bytes, so the camera board handles it in PSRAM; the Nano receives only small result messages. Espressif warns that direct RGB/YUV capture stresses memory, especially with Wi-Fi. Disable Wi-Fi and Bluetooth for normal match operation. See the [official camera driver](https://github.com/espressif/esp32-camera).

For colour, convert sampled cup pixels to HSV or normalised RGB, remove background and very dark or washed-out pixels, then compare against ranges calibrated from the actual red, yellow and green paint. Red needs a hue range spanning the hue wrap. Warm up the camera, settle exposure after a camera-position change and take fresh frames. Lock exposure and white balance for the controlled cup view only after checking that those settings remain usable.

Initial tuning targets are five agreeing fresh frames, at least 60 percent usable cup pixels and at least 80 percent agreement among usable pixels. These are starting thresholds, not a probability of correctness. The desired decision time is under one second; allow up to two seconds for one retry, then preserve UNKNOWN. Test under both room light and strong side light with the actual LEDs and hood.

For acquisition, use colour blobs for patients and dark circular outlines for samples. Calibrate a floor-plane mapping from known positions so pixel location becomes an approximate approach point. Reobserve after short movements instead of driving the whole inferred distance blindly. A raised or tilted patient does not obey the same floor mapping as its centre, so use the base location and close-range confirmation.

Use an acknowledged UART request/result protocol containing request ID, mode, object status, position or colour, and checksum. Accept a response only for the current request and within its deadline. A stale RED result must never classify the next patient. No cloud, laptop or phone is required during a round.

## 7. Routes, field coverage and the two-minute limit

Use coordinates with x measured from the left wall and y from the healthcare end. In the reference page-16 layout, patient rows are at y = 450 and 650 mm; the left sites are approximately x = 150, 250 and 350 mm and the right sites mirror them at x = 793, 893 and 993 mm. Verify these positions against the assembled field rather than deriving a complete map from the schematic.

CareBot starts on the right, collects the right-side sites in a short serpentine route, crosses through the middle and collects the left-side sites. It then takes the healthcare delivery circuit and returns along the right edge to RZ. For Senior, inspect each site before entering it and use the acquisition camera to correct local offsets. Track twelve visited sites separately from twelve successful captures so a missed pickup is revisited rather than counted. Colours are never hard-coded into the route.

SealBot remains in the lower working area, collecting a disc, delivering it, and repeating until all three are seated. It then returns twice for beams and finishes near quarantine. Give CareBot the upper and middle field while SealBot works below it. Check the full camera mast, scoop and carried-beam outlines, not just wheel tracks.

A proposed separation line is y = 800 mm. SealBot's body and mechanisms stay below it after initially leaving start; CareBot stays above it during patient collection and healthcare delivery. This boundary is a route-design target and must be checked against the actual chassis and laboratory approach. CareBot uses the rightmost return corridor for RZ only after its top deliveries, and scans the entrance for SealBot or remaining beams before entering.

Fixed delays alone cannot prevent a collision when one robot runs late. SealBot must clear the start beam staging area by its planned deadline and never return after its second pickup. CareBot checks for an occupied approach and waits or stops if it is blocked. No radio dependency is required for the normal route. Each robot has a low front bumper as a final stop, not as its primary collision planner.

### Timing model, not a measured run

| Robot phase | Proposed timing allowance | What must be measured |
|---|---|---|
| SealBot samples | 0-48 seconds | Three complete acquisition, transport and slot-placement cycles, including rescans. |
| SealBot beams | 48-85 seconds | Two pickups from start, transport, alignment and full release. |
| SealBot finish | 85-100 seconds | Joint inspection, one correction if possible, then park clear. |
| CareBot collect | 0-65 seconds | Exit, travel to twelve targets, inspection and transfer into twelve pockets. |
| CareBot healthcare | 65-95 seconds | Three stops, all kit batches, four red and four yellow patients. |
| CareBot recovery | 95-112 seconds | Right-side return, clear-entry check and four green releases. |
| Both | 112-120 seconds | Margin and automatic final stop. No new long task. |

CareBot's cycle can be checked with a simple sum: path length divided by measured loaded speed, plus turns and docking, plus twelve acquisition cycles, plus magazine motion and twelve releases, plus three kit releases. As an illustrative acceptance budget, 6 m at 0.22 m/s takes 27.3 seconds; 12 seconds for turns, 12 x 3 seconds for acquisition, 24 seconds for patient unloading and rotor travel, and 6 seconds for kit gates totals 105.3 seconds. This leaves only 14.7 seconds for other delays. None of those numbers is a measurement of the proposed robot.

If the route is 8 m or each pickup takes five seconds, the same design exceeds this allowance. Record the actual path and loaded cycle times on day 5, then shorten approaches, overlap safe camera processing with travel, and fix jams. Do not claim all-mission completion by adding two robots' time budgets together; each robot must finish its own critical path within 120 seconds.

Avoid homing the magazine from scratch for every patient. Detect station marks while advancing and inspect the unique home mark whenever it passes. For delivery, one clockwise sweep per destination bounds unnecessary rotation; include the full loaded rotation time in the unloading measurement.

## 8. Electronics, power and pin allocation

Each robot gets one Nano, one ESP32-CAM, one DRV8833 motor driver, two wheel encoders and a five-channel line array using its three central outputs. CareBot additionally gets a PCA9685 servo board and a ULN2003 stepper board. Buy 180-degree positional SG90/MG90S servos, not continuous-rotation substitutes.

Use a protected, preassembled 2S lithium-ion pack per robot and a compatible 8.4 V CC/CV charger. Confirm at least 3 A continuous discharge capability, connector polarity and charger compatibility with the seller. A pack advertised only by capacity is insufficient. Do not use a TP4056 single-cell charger for a 2S pack or solder directly to loose cells.

From each pack, use three independently adjusted buck modules: clean 5 V for the Nano/camera, 5 V for servos and CareBot's stepper, and 5 V for the drive motor supply. Join grounds at a short common distribution point. The separate motor rail prevents applying a fully charged 8.4 V pack directly to nominal 3-6 V BO motors. Verify the motor rail remains usable near the selected low-battery cutoff; an LM2596 is not a zero-dropout regulator.

Fit a fuse and accessible power-stop switch, secure every pack and insulate terminals. Size the fuse from measured load and wire ratings. Place bulk capacitance near servo distribution and the camera, plus local ceramic decoupling. Run camera wiring away from motor leads and solder motor suppression capacitors. Do not feed motors or servos through the Nano regulator or breadboard tracks.

Sequence the lift, gate and stepper movements to limit peak load. The PCA9685 produces control pulses; it does not supply motor power from its logic pin. Route servo current through a suitable separate terminal distribution. An advertised 3 A buck module and a generic DRV8833 label are not evidence that the assembled boards survive the intended load.

The [TI DRV8833 datasheet](https://www.ti.com/lit/ds/symlink/drv8833.pdf) gives package-dependent current limits. Inspect the actual module and test loaded starts, turns and a brief controlled obstruction. If its current or thermal capability is insufficient, buy a suitable driver from reserve before running repeated stalls. Driver sleep must default low during boot.

### Pin allocation: shared Nano connections

| Nano pins | Function | Constraint |
|---|---|---|
| D0 / D1 | Camera UART | Disconnect the link for Nano uploads; no debug text mixed into the protocol. |
| D2 / D3 | Left/right wheel encoders | Interrupt inputs; single-channel direction comes from the commanded motion. |
| D4 / D5 | Left motor input pair | D5 provides PWM. Implement forward/reverse using the driver's truth table. |
| D7 / D6 | Right motor input pair | D6 provides PWM; do not change Timer0 timing. |
| A0 / A1 / A2 | Left/centre/right line outputs | Calibrate black and white on the actual surface. |
| D12 | Start input | Debounced, one activation per power cycle. |
| D13 | DRV8833 sleep/enable | External pulldown; enable only after successful boot checks. |

### Pin allocation: CareBot additions

| Pins or channels | Function | Constraint |
|---|---|---|
| D8-D11 | ULN2003 IN1-IN4 | Non-blocking half-step scheduler. |
| A4 / A5 | PCA9685 SDA / SCL | Short I2C wiring, suitable pullups and separate servo power. |
| PCA channels 0-2 | H, left PCC, right PCC kit gates | Pre-counted whole batches. |
| PCA channel 3 | MG90S patient lift | Calibrated stops; cup stays level. |
| PCA channel 4 | Patient cup floor | Closed during pickup and lift. |
| PCA channel 5 | Magazine outlet gate | Closed during indexing and loading. |
| PCA channel 6 | Camera tilt | Acquisition and inspection stops. |
| A3 | Front bumper | Normally closed contact where practical; open circuit stops motion. |
| A6 | Cup presence | Analog-only Nano input; external bias resistor. |
| A7 | Magazine index sensor | Analog-only input; distinguish home from ordinary station marks. |

### Pin allocation: SealBot additions

| Pins | Function | Constraint |
|---|---|---|
| D8 | MG90S sample lift | Servo library; keep motor PWM on D5/D6. |
| D9 | Sample cup floor servo | Mechanically closed for transport. |
| D10 | Beam clamp servo | Low padded contact; opens fully before withdrawal. |
| D11 / A3 | Left/right contact switches | Distinguish wall alignment from a one-sided impact. |
| A4 | Sample presence switch | Verify occupancy as well as camera detection. |
| A5 | Clamp closed switch | Confirms mechanism state, not necessarily beam capture. |
| A6 | Battery divider | Analog input; scale full-charge voltage safely below 5 V. |
| A7 | Spare analog input | Available for a lift limit or extra presence check. |

CareBot has no dedicated battery ADC in this allocation. Measure its pack before each run and establish a tested minimum starting voltage. Do not quietly assign a second function to A7. The external stop switch removes actuator power independently of program health, while passive gates retain carried pieces if power is lost.

On AI-Thinker-compatible camera boards, GPIO13 and GPIO14 are candidate UART pins when microSD is disabled; verify the exact board schematic before wiring. Reduce Nano TX from 5 V with a suitable divider, for example 1 kilohm above and 2 kilohms to ground. ESP32 GPIO is 3.3 V logic. Check ESP TX reception at the Nano under load; add a proper buffer if signal margins are inadequate. Power the camera through its specified 5 V input, not by applying 5 V to a GPIO.

## 9. Control logic and failure recovery

Use non-blocking state machines. The wheel controller, stepper scheduler, camera timeout and match timer must continue running while a servo settles. Keep image processing on the camera boards; the Nano's small RAM holds only route state and twelve pocket records. Avoid dynamic String allocation in the repetitive control loop.

SealBot's normal sequence is START, FIND_SAMPLE, CAPTURE, VERIFY_CAPTURE, GO_LAB, ALIGN_EMPTY_SLOT, RELEASE, VERIFY_SLOT, repeat three times, FETCH_BEAM, PLACE_BEAM, repeat twice, CHECK_CLOSURE, PARK. CareBot's is START, FIND_PATIENT, CAPTURE, CLASSIFY, INDEX_EMPTY, TRANSFER, VERIFY_TRANSFER, repeat twelve times, DELIVER_LEFT, DELIVER_H, DELIVER_RIGHT, RETURN_RZ, DELIVER_GREEN, PARK.

Each state has a measured timeout, one bounded retry where safe, and a defined stop condition. A camera timeout stops acquisition; an unconfirmed transfer stops the magazine; a bumper stops driving; a line-search timeout stops blind travel. Retrying must not eject an unidentified object or push a sample out of the field.

Inventory is transactional. A pocket changes from EMPTY to a colour only after verified transfer. It changes to EMPTY only after verified unloading. A destination counter increments only after validated delivery. If the camera and presence sensor disagree, record UNCERTAIN and inspect again. Never repair an inventory mismatch by guessing the expected colour counts.

Re-reference odometry at known tape crossings and gentle wall approaches. Use paired sensors or camera line orientation to square the robot. A single line hit gives a position reference but does not remove heading error. Encoders measure wheel rotation, not ground motion, so slipping tyres still require external correction.

At roughly 112 seconds, start no operation whose worst measured completion time exceeds the remaining time. At 119 seconds, stop drive outputs and all indexing commands. Both robots must be stopped by 120 seconds. Retention should be passive so switching off motion does not drop pieces. A reset must boot into a stopped fault state, not resume a route with lost inventory.

## 10. Budget: Rs 9,910 for both robots

These are purchasing allowances including tax where applicable, not a guaranteed delivered shopping cart. Price checks were made on 5-6 September 2026. Consolidate orders and verify stock, delivery date and exact variant. The total includes Rs 400 shipping and Rs 400 repair reserve, leaving Rs 90 below the absolute cap.

| Item | Quantity and allowance | Total Rs |
|---|---|---|
| Complete 2WD chassis sets | 2 x 350; four motors, wheels, casters and hardware | 700 |
| Nano CH340 boards | 2 x 200 | 400 |
| ESP32-CAM with OV2640 | 2 x 720, including 18 percent GST on Rs 610 | 1440 |
| USB-to-TTL programmer | 1 shared | 150 |
| DRV8833 modules | 2 x 80; verify actual current capability | 160 |
| SG90 positional servos | 10 x 100; eight installed, two spare | 1000 |
| MG90S positional servos | 2 x 200; one lift on each robot | 400 |
| 28BYJ-48 plus ULN2003 | 1 set, allowance includes matching driver | 200 |
| PCA9685 board | 1 for CareBot | 260 |
| Five-channel line arrays | 2 x 190; use three outputs each | 380 |
| Wheel encoder sensors and discs | Four complete sensing positions | 200 |
| Contact and magazine sensors | Switches, optical index sensor, flags | 160 |
| Protected 2S battery packs | 2 x 600; confirm discharge rating | 1200 |
| Compatible 8.4 V CC/CV charger | 1 shared; correct connector and polarity | 650 |
| Buck regulator modules | 6 x 60; three rails per robot | 360 |
| Electrical consumables | Fuse, stops, wire, connectors, perfboard, LEDs, capacitors, resistors | 450 |
| Mechanism materials and cutting | Sheet, tubes, axle, spacers, PET, fasteners and local cutting | 650 |
| Practice pieces and tape | Borrow full-size base and tools | 350 |
| Shipping allowance | Consolidated deliveries | 400 |
| Repair reserve | Available only after the complete base purchase | 400 |
| Total | Both robots, practice consumables, shipping and reserve | 9910 |

The mechanism count matches the bill: SealBot has two SG90s and one MG90S. CareBot has six SG90s, one MG90S and one stepper. The remaining two SG90s are spares. Both cameras are included; this is not a one-camera budget attached to a two-camera design.

The small materials allowance assumes school stock and basic workshop access. Buying a new practice table, drill, soldering iron, multimeter or large custom print is outside this total. If those cannot be borrowed, the same all-mission design is not honestly a Rs 10,000 complete project. Source local scrap and cutting before ordering optional extras.

### Price evidence and substitutions

The [Directronics camera listing](https://directronics.in/product/esp32-cam-wifi-module-camera-module) lists Rs 610 excluding 18 percent GST, which is Rs 719.80 per camera. The [Probots chassis listing](https://probots.co.in/2wd-clear-acrylic-smart-robot-chassis-car-kit.html) supplied the approximately Rs 349 chassis reference. Check the delivered wheel envelope and encoder compatibility.

The [Robocraze servo catalogue](https://robocraze.com/collections/servo-motors-controllers) showed SG90 around Rs 95, positional MG90S around Rs 149 and PCA9685 around Rs 255. Allowances are rounded above these references. Its [28BYJ-48 page](https://robocraze.com/products/5v-stepper-motor) showed Rs 83, but the package text and title differ about the driver; budget Rs 200 and explicitly verify that ULN2003 is included.

The [Tomsonelectronics line array](https://www.tomsonelectronics.com/products/5-channel-line-tracking-sensor-module-bfd-1000) supplied the Rs 189 reference; the [Hubtronics encoder sensor](https://hubtronics.in/lm393-speed-measuring-sensor-module) supplied the approximately Rs 40 sensor-only reference. The budget adds allowance for discs. The [Ktron LM2596 listing](https://www.ktron.in/product/lm2596-step-down-module/) supplied the sub-Rs-60 regulator reference.

Battery and charger lines are allowances pending confirmation of a compatible, in-stock pair, not a recommendation to purchase an unverified low-price pack. Keep pack discharge rating, charger chemistry and shipping inside their combined Rs 1,850 allocation or use the repair reserve. Do not hide tax or freight outside the total. Do not buy both an alternative part and the original unless the reserve covers it.

## 11. Nine-day build sequence

Day 1 starts with measuring and making the two difficult mechanisms. One pair of students builds the patient magazine mock-up; another makes the sample cup and actual-size laboratory test; the programming team brings up both cameras and reads the field rules. A teacher supervises cutting and power wiring. If fewer students are available, reduce decorative work and simultaneous experiments, not mission coverage.

| Day | Work | Evidence required before moving on |
|---|---|---|
| 1 | Reproduce the rulebook field; cardboard packing; magazine and sample bench mock-ups; order locally available parts. | Both robots and all initial pieces fit the legal box; horizontal laboratory and beam dimensions reproduced faithfully. |
| 2 | Finish magazine support/indexing and sample lift/floor; test cameras on actual paint; receive electronics. | Twenty consecutive load/index/unload cycles without lost inventory; twenty sample releases into one slot. |
| 3 | Wire safe power rails and chassis; calibrate motors, encoders and line sensors; build rear clamp. | Loaded starts and turns without resets; ten beam releases upright and unsupported. |
| 4 | Add cup transfer, camera tilt and all three kit chutes; make a full-size taped practice half. | 6/2/2 kit drops repeated correctly; test all twelve pockets loaded, not one empty rotor. |
| 5 | Integrate independent complete robot routes, initially without the other robot. | Measured sample cycle, twelve-patient collection time, loaded magazine sweep time and route length. |
| 6 | Complete every mission on both robots; shorten approaches and fix mechanical jams. | One autonomous full-task run per robot inside its 120-second limit. |
| 7 | Run both together with random colour orders and difficult sample placements. | No route conflicts; correct 2/2 yellow split; all three distinct laboratory slots used. |
| 8 | Repeat at least ten full rounds, alternating lighting and battery state; calculate both penalty interpretations. | Log actual outcomes, failure causes and time remaining; no unverified claim of reliability. |
| 9 | Repair only demonstrated issues, secure wires, add SDG3 decoration, pack spares and printed setup sheet. | Final loaded inspection, stop test and reproducible pre-match checklist. |

If day 2's laboratory or magazine check fails, spend day 3 correcting that mechanism before adding more route code. If complete runs still exceed two minutes on day 6, full-score readiness has not been achieved. The plan still includes every mission; match strategy should reflect demonstrated capability rather than pretend the schedule has proved it.

## 12. Failure audit and acceptance tests

| Failure exposed by the architecture | Design response | Acceptance check |
|---|---|---|
| Camera sees colour but robot misses the patient | Separate scene acquisition from presence-confirmed cup inspection. | Test offsets, both rows and all allowed site arrangements. |
| Two objects enter together | Narrow patient cup and sample cup roof; verify occupancy. | Adjacent pieces and touching samples do not create a false single pickup. |
| Colour changes with lighting | Diffused LEDs, repeatable camera stop, calibrated ranges and UNKNOWN. | Test unseen paint samples or orientations under two light conditions. |
| Rotor misses a step or jams | Supported axle, real station marks, one-direction final approach, transfer interlock. | All twelve pockets loaded; repeated complete rotations and selective drops. |
| Wrong pocket inventory | Commit only after verified transfer or release. | Deliberately block one transfer; no later patient is mislabelled. |
| Samples miss their holes | Close camera alignment, measured centres, short vertical drop. | Twenty placements at each slot; include corner sample acquisitions. |
| Beam tips during travel | Force near the floor, low acceleration, full-width route clearance. | Real-weight beams on the actual surface, including turns and release. |
| Beam joint cannot satisfy tape geometry | Full-size wall/tape jig; keep geometric conflict visible and target poses configurable. | Record which contact condition fails; never claim closure bonus from an impossible drawing. |
| Delivered pieces scatter | Short low chute, stationary release, landing margin. | Loaded batch drops with previously delivered pieces present. |
| One robot runs late | Separate normal work areas; visually check shared start access. | Delay SealBot deliberately; CareBot does not collide or push beams. |
| Servo current resets camera | Separate regulated rails, short grounds, sequenced movement. | Loaded drive plus worst intended mechanism action at low starting voltage. |
| Sensor or program stalls | Timeouts, passive retention and independent stop switch. | Disconnect UART or trigger a bumper; motion stops without blind continuation. |
| Full route exceeds the round | Measure each phase and the joint run. | Complete all tasks in less than 112 seconds repeatedly before claiming an eight-second margin. |

A successful bench test proves a mechanism under those conditions. Ten successful rounds are useful evidence, but do not establish a precise long-term failure probability. Record each mission's actual final state and elapsed time, not just whether the robot reached its last waypoint.

Before every round: inspect the field, check pack voltage and all stops, home the empty magazine, verify all gates closed, load exactly 6/2/2 kits, stage both beams, check the full starting envelope and confirm the cameras recognise the actual objects. Remove all setup jigs. Both robots need rounded edges, secured batteries and health/SDG3 decoration. At most three team members enter the competition area, and no unauthorised touching or program restart occurs after activation.
