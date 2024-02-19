# Directory Structure

- `DATASET`: Contains the dataset files for the assignment.
- `SOURCE_CODE`: Contains the source code files for the assignment.

## Dataset

- `mc_train.csv`: Training dataset.
- `mc_test.csv`: Test dataset.

## Source Code

- `data_formatter.py`: Script for formatting the dataset.
- `helper.py`: Helper functions used in the project.
- `label_mappings.json`: JSON file containing label mappings.
- `main.py`: Main script for the assignment.
- `makefile`: Makefile for running the project.
- `mc_train_formatted.csv`: Formatted training dataset.
- `mc_test_formatted.csv`: Formatted test dataset.
- `model.py`: Script containing the model implementation.
- `reverse_label_mapping.json`: JSON file containing reverse label mappings.
- `scikit_learn_tree.py`: Script containing the decision tree implementation using scikit-learn.

# Usage

To run the project, execute the following command from terminal from SOURCE_CODE directory:

```bash
make model
```
To run the project using scikit-learn, execute the following command from terminal from SOURCE_CODE directory:

```bash
make test_scikit_learn
```
To clean the project, execute the following command from terminal from SOURCE_CODE directory:

```bash
make clean
```

# Setup Instructions

1. First Download the dataset and place it in the DATASET directory. Name the training dataset as `mc_train.csv` and test dataset as `mc_test.csv`.

2. Install the required packages using the following command:

```bash
pip install -r requirements.txt
``` 
3. Process the dataset using the data_formatter.py script by setting the **input_file_path = "../DATASET/mc_train.csv"** once and **input_file_path = "../DATASET/mc_test.csv"** once and then run the script using the following command:

```bash
python data_formatter.py
```

4. Run the project using the following command:

```bash
make model
```
# Outputs
A new directory named `OUTPUT` will be created in the SOURCE_CODE directory. The directory will contain two directories named entropy and gini. Each directory will contain the following files:
1. `A directory named "pruned"`: The directory contains images of the pruned decision tree.
2. `A directory named "unpruned"`: The directory contains images of the unpruned decision tree.
3. `Best_pruned.png`: The image of the best pruned decision tree.
4. `Best_unpruned.png`: The image of the best unpruned decision tree.
5. `metrics_pruned.txt`: The file contains the metrics of the pruned decision tree.
6. `metrics_unpruned.txt`: The file contains the metrics of the unpruned decision tree.

The terminal will display the training status and also the best tree for both entropy and gini.

# File Descriptions

- `label_mappings.json`: The JSON file contains mappings between categorical variables and their corresponding numerical representations.
- `reverse_label_mapping.json`: The JSON file contains mappings between numerical representations and their corresponding categorical variables.
- `mc_train_formatted.csv`: The formatted training dataset.
- `mc_test_formatted.csv`: The formatted test dataset.
- `requirements.txt`: The file contains the required packages for the project.
- `data_formatter.py`: The script is used to format the dataset.
- `scikit_learn_tree.py`: The script contains the decision tree implementation using scikit-learn.
- `helper.py`: The script contains helper functions used in the project.
- `model.py`: The script contains the model implementation.
- `main.py`: The main script for the assignment.
- `makefile`: The makefile for running the project.
