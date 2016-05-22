#!/bin/bash

echo "Building plugins..."
sh build_plugins.sh

echo "Generating slide world..."
sh generate_models.sh
