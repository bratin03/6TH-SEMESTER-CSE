"""
Group Number: 23

Group Members:
    Bratin Mondal 
    21CS10016

    Kapil Bhanwariwal
    23BM6JP20

------------------------

Project Number: 1

Project Title: Medical Insurance Approval using Decision Tree based Learning Model
"""


# Importing required libraries
from model import *
import time

# Constants
max_height = 20
# List of attributes
attributes = ['Age', 'Smoke', 'BMI', 'ACC-H', 'Income', 'Med_H']


def select_best_tree(train_file, attributes, criterion='entropy'):
    """
    Function Name: select_best_tree
    Input: File Path, List of Attributes, Criterion
    Output: None
    Purpose: Train the trees with different heights and select the best tree based on accuracy
    """
    print(f"Training trees with {criterion} criterion")
    start = time.time()
    random.seed(int(start))

    print("Loading data...")
    train_csv = read_csv(train_file)
    train_data = process_data(train_csv)
    print("Data loaded.")

    best_test_accuracy_unpruned = 0
    best_test_accuracy_pruned = 0
    best_tree_unpruned = None
    best_tree_pruned = None
    best_height_unpruned = 0
    best_height_pruned = 0

    test_csv = read_csv(TEST_FILE)
    test = process_data(test_csv)
    for height in range(1, max_height+1):
        print(f"\nTraining tree of height {height}")
        train_set, valid_set = train_valid_split(train_data)
        print("Started at: ", time.time()-start, " seconds.")
        Tree = Decision_tree(train_set, attributes, 0, height, criterion)
        print("Tree built at: ", time.time()-start, " seconds.\n")
        train_predictions = predict_batch(Tree, train_set)
        valid_predictions = predict_batch(Tree, valid_set)
        train_accuracy = calculate_accuracy(
            train_predictions, [row['Class'] for row in train_set])
        validation_accuracy = calculate_accuracy(
            valid_predictions, [row['Class'] for row in valid_set])
        test_predictions = predict_batch(Tree, test)
        test_accuracy = calculate_accuracy(
            test_predictions, [row['Class'] for row in test])

        evaluation_metrices = calculate_metrics(
            test_predictions, [row['Class'] for row in test])
        write_metrics_to_file(evaluation_metrices, height,
                              Tree.tree_height(), f"./Output/{criterion}/metrics_unpruned.txt")

        if test_accuracy > best_test_accuracy_unpruned:
            best_test_accuracy_unpruned = test_accuracy
            best_tree_unpruned = Tree.clone()
            best_height_unpruned = height

        graph = graphviz.Digraph(comment='Decision Tree')
        visualize_tree(Tree, graph)
        output_file = f"./Output/{criterion}/unpruned/Height_{height}_unpruned"
        graph.render(output_file, format='png', cleanup=True)
        print("Pruning started at: ", time.time()-start, " seconds.")
        prune_tree(Tree, Tree, valid_set)
        print("Pruning done at: ", time.time()-start, " seconds.\n")
        test_predictions = predict_batch(Tree, test)
        test_accuracy = calculate_accuracy(
            test_predictions, [row['Class'] for row in test])
        if test_accuracy > best_test_accuracy_pruned:
            best_test_accuracy_pruned = test_accuracy
            best_tree_pruned = Tree.clone()
            best_height_pruned = height
        evaluation_metrices = calculate_metrics(
            test_predictions, [row['Class'] for row in test])
        write_metrics_to_file(evaluation_metrices, height,
                              Tree.tree_height(), f"./Output/{criterion}/metrics_pruned.txt")
        graph = graphviz.Digraph(comment='Decision Tree')
        visualize_tree(Tree, graph)
        output_file = f"./Output/{criterion}/pruned/Height_{height}_pruned"
        graph.render(output_file, format='png', cleanup=True)

    print(f"Best tree unpruned: Height: {best_height_unpruned} with accuracy {best_test_accuracy_unpruned}")
    graph = graphviz.Digraph(comment='Decision Tree')
    visualize_tree(best_tree_unpruned, graph)
    output_file = f"./Output/{criterion}/Best_unpruned"
    graph.render(output_file, format='png', cleanup=True)
    print(f"Best tree pruned: Height: {best_height_pruned} with accuracy {best_test_accuracy_pruned}")
    graph = graphviz.Digraph(comment='Decision Tree')
    visualize_tree(best_tree_pruned, graph)
    output_file = f"./Output/{criterion}/Best_pruned"
    graph.render(output_file, format='png', cleanup=True) 

    print("Training complete.")


if __name__ == '__main__':
    """
    main function
    """
    start = time.time()
    random.seed(int(start))
    TRAIN_FILE = 'mc_train_formatted.csv'
    TEST_FILE = 'mc_test_formatted.csv'
    print("Training started at: ", time.time()-start, " seconds.")  
    select_best_tree(TRAIN_FILE, attributes, criterion='entropy')
    print("Training complete at: ", time.time()-start, " seconds.\n")   
    print("Time taken: ", time.time()-start, " seconds.\n")

    print("Training started at: ", time.time()-start, " seconds.")
    select_best_tree(TRAIN_FILE, attributes, criterion='gini')
    print("Training complete at: ", time.time()-start, " seconds.\n")
    print("Time taken: ", time.time()-start, " seconds.\n")
