# Directory Structure

- `DATASET`: Contains the dataset files for the assignment.
- `SOURCE_CODE`: Contains the source code files for the assignment.

## Dataset

- `song.csv`: The dataset for the assignment.
- `train.csv`: The training dataset.
- `test.csv`: The test dataset.

## Source Code

- `Preprocess.py`: The script is used to preprocess the dataset and split it into training and test datasets and save them in the DATASET directory.
- `Model.py`: The script contains the model implementation for the Neural Network along with the different modules as mentioned in the assignment.
- `main.py`: The main script for the assignment.
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

1. First Download the dataset and place it in the DATASET directory. Name the dataset as 'song.csv'.

2. Install the required packages using the following command:

```bash
pip install -r requirements.txt
``` 
3. Process the dataset using `Preprocess.py` script using the following command:
```bash
python3 Preprocess.py
```

4. Run the project using the following command:

```bash
make model
```
<!-- # Outputs
A new directory named `OUTPUT` will be created in the SOURCE_CODE directory.
The directory will contain the following files:

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
- `makefile`: The makefile for running the project. -->