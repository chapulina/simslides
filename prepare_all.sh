#!/bin/bash

echo -e "\e[101mBuilding plugins...\e[49m"
sh build_plugins.sh

echo -e "\e[101mGenerating slide world...\e[49m"
sh generate_models.sh
