from PIL import Image

# === Settings ===
INPUT_FILE = "background.png"  
OUTPUT_FILE = "png.txt"    
TARGET_SIZE = (1920, 1080) 

# === 1️⃣ Loading and conversion ===
print(f"📸 Loading {INPUT_FILE} ...")
img = Image.open(INPUT_FILE).convert("RGB")  # 3 channels only
img = img.resize(TARGET_SIZE, Image.LANCZOS)

print(f"🧩 Resolution: {img.width}x{img.height}")
print("🎨 Formatting: 24-bit RGB (3 bites/pixel)")

# === 2️⃣ Getting pixels ===
pixels = list(img.getdata())  # [(R,G,B), (R,G,B), ...]

# === 3️⃣ Converting to 0xRRGGBB format ===
hex_values = [f"0x{r:02x}{g:02x}{b:02x}" for (r, g, b) in pixels]

# === 4️⃣ Save to hex where each hex is divided by , ===
with open(OUTPUT_FILE, "w") as f:
    # 16 pixels per line for better visualisation
    for i in range(0, len(hex_values), 16):
        f.write(",".join(hex_values[i:i+16]) + ",\n")

print(f"✅ Finnished! RGB24 hex data saved to: {OUTPUT_FILE}")
print(f"💾 Total num of pixels {len(pixels)} ({img.width}x{img.height})")
