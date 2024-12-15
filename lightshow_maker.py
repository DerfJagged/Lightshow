import mido

LED_COLORS_CHANNEL_0 = {}
LED_COLORS_CHANNEL_1 = {}
LED_COLORS_CHANNEL_2 = {}
LED_COLORS_CHANNEL_3 = {}
channel_0_high = 0
channel_1_high = 0
channel_2_high = 0
channel_3_high = 0
channel_0_mid1 = 43
channel_1_mid1 = 43
channel_2_mid1 = 43
channel_3_mid1 = 43
channel_0_mid2 = 86
channel_1_mid2 = 86
channel_2_mid2 = 86
channel_3_mid2 = 86
channel_0_low = 128
channel_1_low = 128
channel_2_low = 128
channel_3_low = 128

def note_to_color(note, channel):
    global channel_0_high
    global channel_1_high
    global channel_2_high
    global channel_3_high
    global channel_0_mid1
    global channel_1_mid1
    global channel_2_mid1
    global channel_3_mid1
    global channel_0_mid2
    global channel_1_mid2
    global channel_2_mid2
    global channel_3_mid2
    global channel_0_low
    global channel_1_low
    global channel_2_low
    global channel_3_low
    global LED_COLORS_CHANNEL_0
    global LED_COLORS_CHANNEL_1
    global LED_COLORS_CHANNEL_2
    global LED_COLORS_CHANNEL_3

    # Map a MIDI note to an LED color
    if (channel == 0):
        for color, note_range in LED_COLORS_CHANNEL_0.items():
            if note in note_range:
                return color
    elif (channel == 1):
        for color, note_range in LED_COLORS_CHANNEL_1.items():
            if note in note_range:
                return color
    elif (channel == 2):
        for color, note_range in LED_COLORS_CHANNEL_2.items():
            if note in note_range:
                return color
    elif (channel == 3):
        for color, note_range in LED_COLORS_CHANNEL_3.items():
            if note in note_range:
                return color
    return "OFF"  # Default if no match

def get_note_ranges(input_file):
    global channel_0_high
    global channel_1_high
    global channel_2_high
    global channel_3_high
    global channel_0_mid1
    global channel_1_mid1
    global channel_2_mid1
    global channel_3_mid1
    global channel_0_mid2
    global channel_1_mid2
    global channel_2_mid2
    global channel_3_mid2
    global channel_0_low
    global channel_1_low
    global channel_2_low
    global channel_3_low
    global LED_COLORS_CHANNEL_0
    global LED_COLORS_CHANNEL_1
    global LED_COLORS_CHANNEL_2
    global LED_COLORS_CHANNEL_3

    midi = mido.MidiFile(input_file)
    for msg in midi.play():  # 'play' accounts for timing between events
        if msg.type == "note_on":
            if msg.channel == 0:
                if (msg.note > channel_0_high):
                    channel_0_high = msg.note
                if (msg.note < channel_0_low):
                    channel_0_low = msg.note
            elif msg.channel == 1:
                if (msg.note > channel_1_high):
                    channel_1_high = msg.note
                if (msg.note < channel_1_low):
                    channel_1_low = msg.note
            elif msg.channel == 2:
                if (msg.note > channel_2_high):
                    channel_2_high = msg.note
                if (msg.note < channel_2_low):
                    channel_2_low = msg.note
            elif msg.channel == 3:
                if (msg.note > channel_3_high):
                    channel_3_high = msg.note
                if (msg.note < channel_3_low):
                    channel_3_low = msg.note
            else:
                continue

    channel_0_mid1 = round(channel_0_low + ((channel_0_high - channel_0_low) / 3))
    channel_1_mid1 = round(channel_1_low + ((channel_1_high - channel_1_low) / 3))
    channel_2_mid1 = round(channel_2_low + ((channel_2_high - channel_2_low) / 3))
    channel_3_mid1 = round(channel_3_low + ((channel_3_high - channel_3_low) / 3))
    channel_0_mid2 = round(channel_0_low + 2*((channel_0_high - channel_0_low) / 3))
    channel_1_mid2 = round(channel_1_low + 2*((channel_1_high - channel_1_low) / 3))
    channel_2_mid2 = round(channel_2_low + 2*((channel_2_high - channel_2_low) / 3))
    channel_3_mid2 = round(channel_3_low + 2*((channel_3_high - channel_3_low) / 3))
    print(f"Channel 0 ranges: {channel_0_low} to {channel_0_high}; mid1={channel_0_mid1}, mid2={channel_0_mid2}")
    print(f"Channel 1 ranges: {channel_1_low} to {channel_1_high}; mid1={channel_1_mid1}, mid2={channel_1_mid2}")
    print(f"Channel 2 ranges: {channel_2_low} to {channel_2_high}; mid1={channel_2_mid1}, mid2={channel_2_mid2}")
    print(f"Channel 3 ranges: {channel_3_low} to {channel_3_high}; mid1={channel_3_mid1}, mid2={channel_3_mid2}")

    LED_COLORS_CHANNEL_0 = {
        "RED": range(channel_0_low, channel_0_mid1),       # Low note range
        "ORANGE": range(channel_0_mid1, channel_0_mid2),   # Mid note range
        "GREEN": range(channel_0_mid2, channel_0_high),    # High note range
    }
    LED_COLORS_CHANNEL_1 = {
        "RED": range(channel_1_low, channel_1_mid1),       # Low note range
        "ORANGE": range(channel_1_mid1, channel_1_mid2),   # Mid note range
        "GREEN": range(channel_1_mid2, channel_1_high),    # High note range
    }
    LED_COLORS_CHANNEL_2 = {
        "RED": range(channel_2_low, channel_2_mid1),       # Low note range
        "ORANGE": range(channel_2_mid1, channel_2_mid2),   # Mid note range
        "GREEN": range(channel_2_mid2, channel_2_high),    # High note range
    }
    LED_COLORS_CHANNEL_3 = {
        "RED": range(channel_3_low, channel_3_mid1),       # Low note range
        "ORANGE": range(channel_3_mid1, channel_3_mid2),   # Mid note range
        "GREEN": range(channel_3_mid2, channel_3_high),    # High note range
    }



def process_midi_file(input_file, output_file):
    midi = mido.MidiFile(input_file)
    if len(midi.tracks) < 4:
        raise ValueError("MIDI file must have at least 4 tracks for 4 LEDs.")

    led_states = ["OFF", "OFF", "OFF", "OFF"]  # Start with all LEDs OFF
    prev_states = led_states[:]  # To track changes
    output_lines = []
    delay_ms = 0

    # Process MIDI messages
    for msg in midi.play():  # 'play' accounts for timing between events
        delay_ms += int(msg.time * 1000) # time since last message
        
        if msg.channel > 3:
            continue

        # Handle note events for channels: 0,1,2,3
        if msg.type == "note_on" and msg.velocity > 0:  # Note pressed
            led_states[msg.channel] = note_to_color(msg.note, msg.channel)  # Map note to color
        elif msg.type == "note_off" or (msg.type == "note_on" and msg.velocity == 0):
            led_states[msg.channel] = "OFF" 

        # Write output only if states change
        if led_states != prev_states and delay_ms != 0:
            output_lines.append(",".join(prev_states) + f",{delay_ms}")
            prev_states = led_states[:]
            delay_ms = 0

    # Turn off when complete
    if led_states != ["OFF", "OFF", "OFF", "OFF"]:
        output_lines.append("OFF,OFF,OFF,OFF,50")

    # Write the output to a file
    with open(output_file, "w") as f:
        for line in output_lines:
            f.write(line + "\n")

# Example usage
input_midi = "song.mid"  # Replace with your MIDI file
output_txt = "lightshow.conf"

try:
    get_note_ranges(input_midi)
    process_midi_file(input_midi, output_txt)
    print(f"Processed LED output saved to {output_txt}")
except Exception as e:
    print(f"Error: {e}")
