from PIL import Image
import numpy as np

def image_to_argb8888_c_array(image_path, output_path):
    try:
        # Open the image using Pillow
        img = Image.open(image_path)

        # Ensure the image is in RGBA mode (4 channels)
        img = img.convert("RGBA")

        # Get the image data as a NumPy array
        data = np.array(img)

        # Flatten the 2D array into a 1D array of uint32_t values (ARGB8888 format)
        argb_array = np.ravel(data.view(dtype=np.uint32))

        # Create the C array declaration
        array_declaration = f"uint32_t image_data[IMAGE_DATA_SIZE] = {{\n"

        # Append each uint32_t value to the array declaration
        for i, val in enumerate(argb_array):
            array_declaration += f"    0x{val:08X},"
            
            # Add newline every 16th value
            if (i + 1) % 16 == 0:
                array_declaration += "\n"

        array_declaration += "\n};\n"

        # Add #define for the number of elements
        num_elements_define = f"#define IMAGE_DATA_SIZE {len(argb_array)}\n"
        array_declaration = num_elements_define + array_declaration

        # Write the C array declaration to the output file
        with open(output_path, "w") as f:
            f.write(array_declaration)

        print("Conversion successful.")
    except Exception as e:
        print(f"Error converting image: {e}")

# Usage example:
image_path = "../ball.png"
output_path = "output_file.c"

image_to_argb8888_c_array(image_path, output_path)
