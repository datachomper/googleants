rm debug.log
python tools/playgame.py "python -m cProfile MyBot.py" "python tools/sample_bots/python/HunterBot.py" --map_file tools/maps/example/tutorial1.map --log_dir game_logs --turns 60 --scenario --food none --player_seed 7 --verbose -e --nolaunch
cat debug.log
