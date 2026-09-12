# Two-robot design for Robotics for Good 2026-2027

**Concept:** SealBot and CareBot  
**Rule source:** [Rulebook-2026.pdf](C:/Users/USER/Desktop/ihfc/Rulebook-2026.pdf)

## The idea

Build two small autonomous robots with different jobs. SealBot handles the quarantine zone, containment beams, and samples. CareBot delivers medical kits and sorts patients by colour.

The JCB and lift ideas are good, but a full robotic arm is unnecessary. These tasks happen close to the floor and reward accurate placement more than free movement. Short scoops, a belt lift, gravity-fed storage, and simple gates will be easier to build and more reliable within a two-minute match.

The robots should share the same drive base, controller, motors, wheels, and navigation code. Only their upper mechanisms differ. This cuts development time and lets the team carry one set of common spares.

## Rulebook constraints

Both robots and every preloaded game piece must fit together inside the 480 × 280 mm starting zone. The proposed loaded arrangement uses about 450 × 280 mm.

Each robot must run autonomously after the start and must have an accessible emergency stop. Both must also have health-emergency or SDG 3 decoration, protected batteries, covered wiring, rounded edges, and no loose parts.

The important game pieces are:

- Three wooden sample discs, each 56 mm in diameter and 5 mm thick.
- A laboratory with three 60 mm openings.
- Two containment beams measuring 250 × 60 × 20 mm and 280 × 60 × 20 mm.
- Ten medical kits measuring 25 × 25 × 20 mm.
- Twelve patient cylinders measuring 20 mm in diameter and 20 mm high. Four are red, four yellow, and four green.

The Senior category randomises sample positions and patient colours. The robot must find samples across the quarantine area and read each patient's colour instead of relying on a memorised order.

## Overall layout

```text
480 × 280 mm starting zone

┌──────────────────────────────────────────────┐
│ SealBot            gap          CareBot     │
│ 215 × 280 mm                    225 × 250 mm│
│ beams and samples               kits/lift   │
└──────────────────────────────────────────────┘
```

The 280 mm beam exactly matches the nominal depth of the starting zone. Mount it at a slight angle or diagonally above SealBot so manufacturing and tape tolerances do not make the starting arrangement illegal.

Both robots use two-wheel differential drive. Each base has two driven rubber wheels, two small ball casters, wheel encoders, a line-sensor bar, and two soft wall feelers. A suitable chassis is roughly 210 mm wide and 230 mm long. Keep the battery low and near the drive axle.

The robots should move at moderate speed across open space, then slow down near game pieces and destination zones. Fast movement saves little time if the robot needs several attempts to align.

## SealBot

SealBot completes the quarantine perimeter and transfers all three samples to the laboratory. It works mainly in the lower part of the field, away from CareBot's initial route.

### Beam mechanism

Carry each beam upright inside a U-shaped side cassette. The cassette keeps the beam standing while a small servo latch prevents it from sliding out.

To place a beam, SealBot follows these steps:

1. Drive to the marked beam line.
2. Detect the black tape and slow down.
3. Touch the nearby wall with two spring-loaded feelers. When both switches close, the robot is square to the wall.
4. Open the servo latch.
5. Reverse straight by about 35 mm, leaving the beam standing on its long edge.
6. Pause before turning so the chassis cannot knock the beam over.

SealBot places the 280 mm beam first, rotates 90 degrees, and then places the 250 mm beam. The cassette position and route must make the two beam ends touch while each beam also touches a fixed wall. After the second release, the robot moves away so neither beam remains supported by it.

This mechanism is simpler than gripping and rotating each beam with an arm. The field walls provide the final alignment, while the cassette controls the beam's upright orientation.

### Sample mechanism

The three sample discs may appear anywhere inside the quarantine area. SealBot therefore uses a wide front intake rather than trying to locate and grip each disc separately.

The intake has:

- A thin rounded lip that slides under a flat sample.
- A soft foam roller that pulls the sample into the robot.
- A funnel that narrows the intake into a 60 mm storage channel.
- A tray that holds three discs in a horizontal stack.
- A servo shutter that releases one disc at a time.

SealBot makes two overlapping passes through the 280 × 280 mm quarantine area. A 145 mm intake covers the whole area in two lanes with a small overlap. A switch inside the tray counts the collected discs. The intake stops after three discs or after the sweep time expires.

At the laboratory, the robot aligns using the black marking and a physical edge reference. A removable funnel sits above the first opening. Its lower opening should be around 58 mm, which centres a 56 mm disc over the 60 mm laboratory slot. SealBot releases one sample, moves by the measured slot spacing, and repeats for the other two.

The laboratory drawing does not make its installed orientation completely clear. Build a physical replica before freezing this mechanism. If samples must enter a vertical opening, replace the drop funnel with a short two-axis lift that raises the disc and rotates it through 90 degrees. Keep the same chassis and mounting holes.

## CareBot

CareBot delivers the ten medical kits first. It then collects, identifies, stores, and delivers the twelve patient cylinders.

### Medical-kit mechanism

Load the kits into three separate gravity-fed magazines before the match:

- Two kits for the first primary care centre.
- Six kits for the hospital.
- Two kits for the second primary care centre.

Each magazine has a servo gate at the bottom. CareBot visits the three destinations and opens only the matching gate. A small exit switch confirms that each block has left the chute.

The robot should deliver one kit to every destination before completing the full distribution. In the Senior category, leaving a destination empty can cause a penalty. This sequence protects the score if a later gate jams or time runs short.

### Patient collection and lift

The patient locations are known, but their colours may be random. CareBot visits all twelve marked positions and collects one cylinder at a time.

A shallow JCB-style scoop centres the upright cylinder into a 22 to 23 mm channel. A narrow cleated belt then lifts it into a covered colour-reading chamber. The channel prevents two cylinders from entering together and keeps each one upright.

Inside the chamber, a fixed white LED illuminates the cylinder and a colour sensor reads its surface. The chamber should be matte black so changing room light has little effect. The belt stops during measurement, and the controller takes several readings before choosing a colour.

A two-servo chute sends the cylinder into one of four storage bins:

- Red bin with capacity for four cylinders.
- Green bin with capacity for four cylinders.
- Yellow-left bin with capacity for two cylinders.
- Yellow-right bin with capacity for two cylinders.

Accepted yellow cylinders alternate between the two yellow bins. This makes the required two-and-two distribution automatic.

If the colour reading is uncertain, CareBot keeps the cylinder in a reject pocket instead of guessing. A wrong delivery loses points in the Senior category, while a retained piece only misses its possible score.

After collecting the patients, CareBot releases the red bin at the hospital, one yellow bin at each primary care centre, and the green bin inside the recovery zone.

## Navigation

The field has known dimensions and strong black markings, so the robots do not need cameras for normal navigation.

Each movement uses three layers of correction:

1. Wheel encoders control straight travel and turns.
2. Line sensors correct position when the robot reaches a marked zone.
3. Wall feelers or a short-range distance sensor provide the final alignment.

Open-loop timed movement should only be used for very short mechanism actions. Battery voltage, payload, wheel slip, and field material will otherwise change the distance travelled.

Store all field distances, turning angles, sensor thresholds, and servo positions as configuration values. The team can then adjust the robots during the event's adaptation period without rewriting the mission logic.

The two robots should not depend on wireless communication. Give them separate routes and time windows:

- SealBot owns the quarantine corner, laboratory edge, and lower-left area.
- CareBot leaves through the centre-right lane and handles the upper healthcare zones first.
- CareBot returns to the recovery zone only near the end of the match.

If both routes cross one narrow area, add a fixed wait to one robot. Predictable timing is safer than radio coordination during a short match.

## Match sequence

### SealBot target sequence

| Phase | Target time |
|---|---:|
| Exit and place the first beam | 12 seconds |
| Place the second beam | 16 seconds |
| Sweep and collect samples | 28 seconds |
| Align with the laboratory | 10 seconds |
| Place three samples | 15 seconds |
| Spare time and safe stop | 39 seconds |

### CareBot target sequence

| Phase | Target time |
|---|---:|
| Deliver one kit to every destination | 20 seconds |
| Complete the kit distribution | 15 seconds |
| Collect and sort twelve patients | 42 seconds |
| Deliver red and yellow patients | 25 seconds |
| Return and release green patients | 13 seconds |
| Stop margin | 5 seconds |

Both robots should stop all motors by 118 seconds and remain still for scoring.

## Control software

Use a separate state machine for each robot. Every state needs a success condition, a time limit, and a safe failure path. Avoid long blocking delays because the emergency logic and match timer must keep running while a mechanism moves.

SealBot states:

```text
Self-test
Start
Place long beam
Place short beam
Sweep samples twice
Align with laboratory
Release three samples
Park and stop
```

CareBot states:

```text
Self-test
Start
Deliver medical kits
Collect left-side patients
Collect right-side patients
Deliver red patients
Deliver yellow patients
Deliver green patients
Park and stop
```

If a mechanism fails, the robot should try one short recovery and then skip that action. Repeating the same failed movement can consume the remaining match.

CareBot should have a software option that disables triage. Keep triage disabled in competition until medical-kit delivery succeeds in at least 19 of 20 full tests. This gives the team a reliable scoring robot even if the lift is still being tuned.

## Safety and power

Each robot needs a protected battery, fuse, latching red emergency stop, separate start button, and regulated power rails for motors, servos, and logic.

The emergency stop must physically remove power from the motors and servos. Software alone cannot be trusted if the controller freezes. Mount the stop on top where a referee can reach it quickly.

Do not power servos through the controller board. Their current spikes can reset the processor. Use a separate regulator, join all grounds at one point, and place large capacitors near the servo rail and motor driver.

Secure every cable and battery. Cover exposed gears and belt pinch points. Add felt, silicone, or smooth plastic to anything that may touch the field or game pieces.

## Build and testing plan

1. Build the starting zone, quarantine corner, beams, samples, laboratory, and destination markings at full size.
2. Build one common drive base and tune straight movement, 90-degree turns, line detection, and wall alignment.
3. Test the beam cassette until it places both beams correctly in at least 48 of 50 attempts.
4. Test sample collection with random legal layouts, then test the laboratory funnel separately.
5. Complete SealBot and repeat full autonomous runs until it is stable.
6. Build CareBot's medical-kit magazines and secure the kit mission before adding patient handling.
7. Test the patient scoop, lift, colour chamber, and sorter as a stationary mechanism before mounting them on the robot.
8. Run both robots together and adjust routes to remove collisions.
9. Freeze the mechanical design two weeks before competition and spend the remaining time on calibration and full-match practice.

The design passes when both loaded robots fit legally, every emergency stop works, the robots stop before 120 seconds, SealBot completes its beam and sample tasks repeatedly, and CareBot delivers the medical kits without depending on the triage system.

## Final recommendation

Build the two specialised robots. SealBot should use upright beam cassettes, a wide sample roller, and a precise drop funnel. CareBot should use three preloaded kit magazines and a JCB-style scoop feeding a compact colour-sorting lift.

Do not start with a full robotic arm. Add the small two-axis sample lift only if a laboratory replica proves that the simpler funnel cannot place the discs. The strongest first target is a repeatable run covering both beams, all samples, and all medical kits. Add patient sorting after that core works reliably.
