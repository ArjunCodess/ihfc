# CareBot: keep the original design

Build the CareBot described in the supplied illustration prompt. Its patient mechanism is a fixed scoop, a retaining gate, a padded ejector and a fixed ESP32-CAM. The patient stays in the same pocket throughout the trip.

SCOOP → HOLD → INSPECT → DRIVE → EJECT

## The mechanism

- Use the ordinary two-wheel chassis with a rear caster. Keep the battery low and the electronics protected.
- Fix the shallow scoop to the chassis. The JCB resemblance describes its shape; the scoop does not tilt or lift.
- Make the entrance 45–50 mm wide and the pocket approximately 24–26 mm wide for one upright 20 × 20 mm patient. Trial these clearances with the painted pieces before fixing the walls.
- Use one SG90 for the front gate. It opens fully out of the entrance and closes only after the patient is seated behind it.
- Use a second SG90 for the padded ejector behind the pocket. It pushes the patient through the open entrance at delivery.
- Fix the ESP32-CAM above the pocket, looking down at the patient's painted top or broad face. Keep the dark hood and two diffused white LEDs. There is no moving camera.

## Improve the existing parts, not the architecture

1. Keep the entrance nearly flush with the board. A rounded thin PET lip and gradual transition reduce the chance of tipping the patient against a steep step. Do not make a deep excavator bucket.
2. Give the gate a physical closed-position stop and clearance around the patient. It should block escape, not squeeze the cylinder or require the servo to stall.
3. Guide the ejector between simple straight rails. Its padded face should push low on the cylinder and travel far enough to clear the lip. An unguided servo horn can sweep the patient sideways.
4. Make the pocket interior plain and matte. Keep coloured wires, kit markings and direct LED reflections outside the camera's inspection crop.
5. Add one light presence switch. Drive stops before inspection; the gate closes only after seating. If the patient is missing or the colour is uncertain, do not guess.

At delivery: stop, open the gate fully, extend the ejector, retract it, confirm the pocket is empty, then reverse clear. Test this complete sequence before writing a long route.

## The other task on CareBot

Keep the previous three preloaded kit chutes with their existing gates: six kits for H and two for each PCC. They mount behind or above the chassis, clear of the camera and scoop. Kits never enter the patient pocket. The patient mechanism has two servos; the complete CareBot has five when the three kit gates are included.

No conveyor, elevator, carousel, sorting bins, turntable, patient transfer, arm, laboratory mechanism or camera movement belongs on this CareBot. Sample handling remains a separate SealBot problem and is not a reason to alter this design.

Build one working scoop assembly from PET and offcuts first. Repeat pickup, inspection and ejection with the actual-size cylinder. Fix the lip, gate clearance or ejector guide if a cycle fails; keep the same mechanism. This brief freezes CareBot's architecture. Competition speed and reliability still require testing.
