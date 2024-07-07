# Path to input and output CSV files
input_file = "raw data\cooldown.csv"
output_file = "ESA_one_watt_dual_Fixed.csv"

# Read input CSV file line by line into a list
data = []
with open(input_file, "r") as f:
    for line in f:
        if len(line) > 1:
            data.append(
                line.strip().split(";")
            )  # Splitting each line by ';' and stripping whitespace


# Display the first few rows and column names for debugging purposes
print("Data read from CSV:")  # Displaying first 5 rows
for el in data[:10]:
    print(el)

header = data[0]


# delete header
data = filtered_data = [row for row in data if row[0] != "timestamp[ms]"]

# Example: erase the time offset



# def adjust_data(data):
#     if len(data) == 0:
#         raise ValueError("Data is empty")
#     try:
#         time_offset = 0  # Initialize time offset
#         header_skipped = False  # Flag to track if header has been skipped

#         for i in range(len(data)):
#             # Skip headers until a valid numeric timestamp is found
#             if not header_skipped and not data[i][0].isdigit():
#                 continue

#             header_skipped = True  # Mark that headers have been skipped

#             # Convert timestamp to integer
#             current_timestamp = int(data[i][0])

#             if i > 0 and current_timestamp < int(data[i - 1][0]):
#                 time_offset = int(data[i - 1][0])
#                 # print(f'Time offset updated: {time_offset}')

#             data[i][0] = str(current_timestamp + time_offset)
#             yield data[i]

#     except Exception as e:
#         print(f"Error occurred: {e}")
#         print(f"Data[{i}][0] < Data[{i - 1}][0]: {data[i][0]} < {data[i - 1][0]}")


def adjust_data(data):
    time_offset = 0  # Initialize as string to match data type
    try:
        for i in range(len(data)):
            data[i][0] = int(data[i][0]) + time_offset
            if i != 0 and i != 1 and int(data[i][0]) < int(data[i - 1][0]):
                time_offset += int(data[i - 1][0])
                print(f'timeoffset: {time_offset}')
                data[i][0] += time_offset
            
            yield data[i]
    except Exception as e:
        print("oi y cunt")
        print(f"Error occurred: {e}")
        print(f"Data[{i}][0] < Data[{i - 1}][0]: {data[i][0]} < {data[i - 1][0]}")
        print(f'{data[i][0]} < {data[i - 1][0]}')

# Creating a generator instance
adjusted_data_generator = adjust_data(data)

# Iterating through the generator to modify data
for i, modified_row in enumerate(adjusted_data_generator):
    data[i] = modified_row

# Now data has been modified in place
# print(data)

print("data prozessing done lets start saving:")
print("add header back")

# for el in data[:10]:
#     print(el)

# data =[ 'timestamp[ms]', 'temp_probe[°C]', 'temp_out[°C]'] + data

data = [header] + data

# Save modified data to a new CSV file
with open(output_file, "w") as f:
    for row in data:
        for el in row:
                if(el) is not '':
                    f.write(str(el))
                    f.write(";")
        f.write("\n")



print("Modified data saved to:", output_file)
