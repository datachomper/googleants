#!/usr/bin/env sh

ENGINE="/home/russ/projects/googleants/tools/playgame.py"
MYBOT="/home/russ/projects/googleants/c/MyBot"
OTHERBOT="python /home/russ/projects/googleants/tools/sample_bots/python/HunterBot.py"
#MAP="/home/russ/projects/googleants/tools/maps/example/tutorial1.map"
MAP="/home/russ/projects/googleants/tools/maps/maze/maze_02p_02.map"

${ENGINE} --log_dir log --log_input --turns 500 --player_seed 42 --verbose -E --fill --turntime 500 --map_file ${MAP} ${MYBOT} "${OTHERBOT}"

#python ../tools/playgame.py "./MyBot" "python ../tools/sample_bots/python/HunterBot.py" --map_file ../tools/maps/example/tutorial1.map --log_dir log --log_input --turns 60 --scenario --food none --player_seed 7 --verbose -E --end_wait 0.25
#cat ./log/0.bot0.error
