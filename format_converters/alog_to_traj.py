import os
import glob


def split_coord_line(line):
    split_line = line.split(" ")
    split_line = [i for i in split_line if i != ""]
    return split_line[0], split_line[-2]


def convert_alog_to_traj():
    # Determine which files to work on
    source_filepaths = []
    for name in glob.glob("../missions/lcgp_test/LOG_AGENT*/*.alog"):
        source_filepaths.append(name)

    # Setup
    trajs = []
    last_timestep = 0
    for source_filepath in source_filepaths:
        trajs.append([])
        start_pos = [0.0, 0.0, 0.0]
        current_x = 0.0
        current_y = 0.0
        new_coords = [False, False]
        started = False
        print("Processing:", source_filepath)

        with open(source_filepath, "r") as filehandle:
            stopped = False

            # Isolate relevant lines and organize by time
            for line in filehandle:
                # Update coords
                if "TRUE_X" in line:
                    time, current_x = split_coord_line(line)
                    new_coords[0] = True

                elif "TRUE_Y" in line:
                    time, current_y = split_coord_line(line)
                    new_coords[1] = True

                # Reached endpoint
                elif "MODE@ACTIVE:STATION-KEEPING" in line and not stopped:
                    last_timestep = max(last_timestep, len(trajs[-1]))
                    stopped = True

                # Handle new x & y
                if all(new_coords):
                    new_coords = [False, False]

                    # First value
                    if len(trajs[-1]) == 0:
                            trajs[-1].append([time, current_x, current_y])

                    # Once the agent starts moving
                    elif start_pos[1] != current_x or start_pos[2] != current_y:
                        trajs[-1].append([time, current_x, current_y])

    # Formatting Cleanup
    for traj_index in range(len(trajs)):
        trajs[traj_index] = trajs[traj_index][:last_timestep]
        for i in range(last_timestep):
            x = float(trajs[traj_index][i][1])
            y = float(trajs[traj_index][i][2])

            # change range
            y += 200

            # scale
            factor = 35/200
            x *= factor
            y *= factor

            trajs[traj_index][i] = (x,y)

    # Make output name
    intermediate_step = source_filepaths[0][source_filepaths[0].index("AGENT") + 5: source_filepaths[0].index(".alog")]
    timestamp = intermediate_step[intermediate_step.index("_") + 1: intermediate_step.index("/")]

    # Setup output file
    num_only_timestamp = timestamp.replace("_", "")
    traj_filepath = "./trajs/" + "traj_" + num_only_timestamp + ".txt"

    file_dir = os.path.dirname(traj_filepath)
    if not os.path.isdir(file_dir):
        os.mkdir(file_dir)

    # Write to output
    with open(traj_filepath, "w") as filehandle:
        for traj in trajs:
            filehandle.write("%s\n" % traj)


if __name__ == "__main__":
    # Small case initially
    convert_alog_to_traj()
