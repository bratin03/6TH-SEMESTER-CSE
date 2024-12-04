"""
Student Name: Bratin Mondal
Student ID: 21CS10016
Project Code: CGHC-DC
Project Title: Country Grouping using Complete Linkage Divisive (Top-Down) Clustering Technique
"""

import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import time


def cosine_similarity(v1, v2):
    """
    Calculates the cosine similarity between two vectors.

    Parameters:
    v1 (numpy.ndarray): The first vector.
    v2 (numpy.ndarray): The second vector.

    Returns:
    float: The cosine similarity between the two vectors.
    """
    return np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))


def DataLoader(file_path="../DATASET/country.csv"):
    """
    Load data from a CSV file and return the attributes and entries.

    Parameters:
    file_path (str): The path to the CSV file. Default is "../DATASET/country.csv".

    Returns:
    tuple: A tuple containing the attributes (column names) and entries (data values) from the CSV file.
    """
    data = pd.read_csv(file_path).dropna()
    attributes = data.columns
    entries = data.values
    return attributes, entries


def sort_clusters(names_clusters, clusters):
    """
    Sorts the names_clusters and clusters based on the names in ascending order.

    Args:
        names_clusters (list): A list of lists containing names for each cluster.
        clusters (list): A list of lists containing data for each cluster.

    Returns:
        tuple: A tuple containing the sorted names_clusters and sorted clusters.
    """
    sorted_clusters = []
    for i in range(len(names_clusters)):
        sorted_clusters.append([])
        for name in sorted(names_clusters[i]):
            index = names_clusters[i].index(name)
            sorted_clusters[i].append(clusters[i][index])
    sorted_names_clusters = [sorted(names) for names in names_clusters]
    return sorted_names_clusters, sorted_clusters


def K_Means_Clustering(k, entries, itr=20, random_seed=2):
    """
    Perform K-means clustering on a given dataset.

    Parameters:
    - k (int): The number of clusters to create.
    - entries (list): A list of entries, where each entry is a list of features.
    - itr (int): The maximum number of iterations to perform. Default is 20.
    - random_seed (int): The random seed for centroid initialization. Default is 2.

    Returns:
    - names_clusters (list): A list of lists, where each inner list contains the names of entries in a cluster.
    - clusters (list): A list of lists, where each inner list contains the entries in a cluster.
    - centroids (ndarray): An array of centroid vectors representing the final centroids of each cluster.
    """
    entry_without_country = [entry[1:] for entry in entries]
    names = [entry[0] for entry in entries]
    np.random.seed(random_seed)
    centroids = np.array(
        [
            entry_without_country[i]
            for i in np.random.choice(len(entry_without_country), k, replace=False)
        ]
    )

    for _ in range(itr):
        clusters = [[] for _ in range(k)]
        names_clusters = [[] for _ in range(k)]
        for i, entry in enumerate(entry_without_country):
            max_similarity = -1
            cluster_index = -1
            for j, centroid in enumerate(centroids):
                similarity = cosine_similarity(entry, centroid)
                if similarity > max_similarity:
                    max_similarity = similarity
                    cluster_index = j
            clusters[cluster_index].append(entry)
            names_clusters[cluster_index].append(names[i])
        new_centroids = [np.mean(cluster, axis=0) for cluster in clusters]
        if np.array_equal(centroids, new_centroids):
            break
        centroids = new_centroids
    return names_clusters, clusters, centroids


def write_clusters_to_file(names_clusters, clusters, centroids, file_path):
    """
    Write the clusters and their information to a file.

    Args:
        names_clusters (list): A list of lists containing the names of countries in each cluster.
        clusters (list): A list of lists containing the attribute values of countries in each cluster.
        centroids (list): A list of lists containing the centroid values of each cluster.
        file_path (str): The path to the file where the clusters will be written.

    Returns:
        None
    """
    with open(file_path, "w") as f:
        f.write("Total Clusters: {}\n".format(len(clusters)))
        for i in range(len(clusters)):
            f.write("Cluster {}\n".format(i + 1))
            f.write("Number of Countries: {}\n".format(len(clusters[i])))
            centroid_str = " ".join(map(str, centroids[i]))
            f.write("Centroid: {}\n".format(centroid_str))

            for j in range(len(names_clusters[i])):
                f.write(names_clusters[i][j] + "\n")
                cluster_str = " ".join(map(str, clusters[i][j]))
                cluster_str = "Attributes: " + cluster_str
                f.write(cluster_str + "\n")
            f.write("\n")


def read_clusters_from_file(file_path):
    """
    Read clusters from a file and return the names and clusters.

    Args:
        file_path (str): The path to the file containing the clusters.

    Returns:
        tuple: A tuple containing two lists. The first list contains the names of the clusters,
               and the second list contains the clusters.

    """
    with open(file_path, "r") as f:
        lines = f.readlines()
        num_clusters = int(lines[0].split(": ")[1])
        names_clusters = []
        clusters = []
        for i in range(1, num_clusters + 1):
            names_clusters.append([])
            clusters.append([])
        cluster_index = 0
        for i in range(1, len(lines)):
            if lines[i].startswith("Cluster"):
                cluster_index += 1
            elif lines[i].startswith("Number"):
                continue
            elif lines[i].startswith("Centroid"):
                continue
            elif lines[i] == "\n":
                continue
            elif lines[i].startswith("Attributes"):
                cluster = lines[i].split(": ")[1].split()
                cluster = [float(x) for x in cluster]
                clusters[cluster_index - 1].append(cluster)
            else:
                name = lines[i].strip()
                names_clusters[cluster_index - 1].append(name)
    return names_clusters, clusters


def calculate_Silhouette(clusters):
    """
    Calculates the Silhouette coefficient for a given set of clusters.

    Parameters:
    clusters (list): A list of clusters, where each cluster is represented as a list of entries.

    Returns:
    float: The Silhouette coefficient.

    """
    silhouette = 0
    sample_size = sum([len(cluster) for cluster in clusters])
    for i, cluster in enumerate(clusters):
        for entry in cluster:
            a = np.mean(
                [
                    1 - cosine_similarity(entry, entry2)
                    for entry2 in cluster
                    if not np.array_equal(entry, entry2)
                ]
            )
            b = min(
                [
                    np.mean(
                        [1 - cosine_similarity(entry, entry2) for entry2 in clusters[j]]
                    )
                    for j in range(len(clusters))
                    if j != i
                ]
            )
            silhouette += (b - a) / max(a, b)
    silhouette /= sample_size
    return silhouette


def find_best_k(entries):
    """
    Finds the best value of k for K-Means clustering based on the silhouette score.

    Parameters:
    entries (list): A list of entries to be clustered.

    Returns:
    int: The best value of k.

    """
    best_k = 0
    best_silhouette = -1
    for k in range(3, 7):
        cur_time = time.time()
        print("=====================================")
        print("Clustering with K =", k)
        names_clusters, clusters, centroids = K_Means_Clustering(k, entries, itr=20)
        write_clusters_to_file(
            names_clusters,
            clusters,
            centroids,
            "../OUTPUT/cluster_output_{}.txt".format(k),
        )
        silhouette = calculate_Silhouette(clusters)
        print("=====================================")
        print("K:", k, "\nSilhouette:", silhouette)
        print(f"Cluster Output File: cluster_output_{k}.txt")
        print("Time Taken:", time.time() - cur_time)
        print("=====================================")
        if silhouette > best_silhouette:
            best_silhouette = silhouette
            best_k = k
    return best_k


def Hierarchical_Divisve_Clustering(entries, number_of_clusters=6):
    """
    Perform Hierarchical Divisive Clustering on a given dataset.

    Args:
        entries (list): A list of entries, where each entry is a list of features.
        number_of_clusters (int): The number of clusters to create. Default is 6.

    Returns:
        tuple: A tuple containing the following:
            - names_clusters (list): A list of lists, where each inner list contains the names of entries in a cluster.
            - value_clusters (list): A list of lists, where each inner list contains the attribute values of entries in a cluster.
            - centroids (ndarray): An array of centroid vectors representing the final centroids of each cluster.
    """
    clusters = []
    clusters.append(entries)
    while len(clusters) < number_of_clusters:
        max_dissimilarity = -1
        cluster_index_to_split = -1
        for i in range(len(clusters)):
            total_pair = 0
            dissimilarity = 0
            for j in range(len(clusters[i])):
                for k in range(j + 1, len(clusters[i])):
                    total_pair += 1
                    dissimilarity += 1 - cosine_similarity(
                        clusters[i][j][1:], clusters[i][k][1:]
                    )
            dissimilarity /= total_pair
            if dissimilarity > max_dissimilarity:
                max_dissimilarity = dissimilarity
                cluster_index_to_split = i
        cluster_to_split = clusters[cluster_index_to_split]
        leader_index = 0
        max_dissimilarity = -1
        for i in range(len(cluster_to_split)):
            dissimilarity = 0
            for j in range(len(cluster_to_split)):
                dissimilarity += 1 - cosine_similarity(
                    cluster_to_split[i][1:], cluster_to_split[j][1:]
                )
            dissimilarity /= len(cluster_to_split)
            if dissimilarity > max_dissimilarity:
                max_dissimilarity = dissimilarity
                leader_index = i
        leader = cluster_to_split[leader_index]
        new_cluster1 = []
        new_cluster2 = []
        for i in range(len(cluster_to_split)):
            if i == leader_index:
                new_cluster1.append(cluster_to_split[i])
            else:
                distance_from_leader1 = 1 - cosine_similarity(
                    cluster_to_split[i][1:], leader[1:]
                )
                distance_from_cluster = 0
                for j in range(len(cluster_to_split)):
                    if j != leader_index:
                        distance_from_cluster = max(
                            distance_from_cluster,
                            1
                            - cosine_similarity(
                                cluster_to_split[i][1:], cluster_to_split[j][1:]
                            ),
                        )
                if distance_from_leader1 < distance_from_cluster:
                    new_cluster1.append(cluster_to_split[i])
                else:
                    new_cluster2.append(cluster_to_split[i])
        clusters.pop(cluster_index_to_split)
        clusters.append(new_cluster1)
        clusters.append(new_cluster2)
    names_clusters = []
    for cluster in clusters:
        names_clusters.append([entry[0] for entry in cluster])
    value_clusters = []
    for cluster in clusters:
        value_clusters.append([entry[1:] for entry in cluster])
    centroids = []
    for cluster in value_clusters:
        centroids.append(np.mean(cluster, axis=0))
    return names_clusters, value_clusters, centroids


def generate_permutations(n):
    """
    Generate all permutations of numbers from 0 to n-1.

    Args:
        n (int): The number of elements to generate permutations for.

    Returns:
        list: A list of lists, where each inner list represents a permutation.

    Examples:
        >>> generate_permutations(3)
        [[0, 1, 2], [1, 0, 2], [1, 2, 0], [0, 2, 1], [2, 0, 1], [2, 1, 0]]
    """
    if n == 1:
        return [[0]]
    permutations = []
    for i in range(n):
        sub_permutations = generate_permutations(n - 1)
        for sub_permutation in sub_permutations:
            new_permutation = sub_permutation.copy()
            new_permutation.insert(i, n - 1)
            permutations.append(new_permutation)
    return permutations


def Jaccard_Similarity(list1, list2):
    """
    Calculates the Jaccard similarity coefficient between two lists.

    The Jaccard similarity coefficient is a measure of the similarity between two sets.
    It is defined as the size of the intersection divided by the size of the union of the sets.

    Args:
        list1 (list): The first list.
        list2 (list): The second list.

    Returns:
        float: The Jaccard similarity coefficient between the two lists.
    """
    intersection_count = 0
    union_count = len(list1) + len(list2)

    for item in list1:
        if item in list2:
            intersection_count += 1
            union_count -= 1

    if union_count == 0:
        return 0.0
    else:
        return intersection_count / union_count
    
def Jaccard_Similarity_Visualization(names_clusters_kmeans, names_clusters_hierarchical):
    """
    Displays a heatmap of the Jaccard similarity between two sets of clusters.
    
    Args:
        names_clusters_kmeans (list): A list of lists containing the names of countries in each cluster for K-Means clustering.
        names_clusters_hierarchical (list): A list of lists containing the names of countries in each cluster for Hierarchical clustering.
    
    Returns:
        ndarray: A 2D numpy array containing the Jaccard similarity scores between the two sets of clusters.
    """
    n = len(names_clusters_kmeans)
    m = len(names_clusters_hierarchical)
    jaccard_matrix = np.zeros((n, m))
    for i in range(n):
        for j in range(m):
            jaccard_matrix[i][j] = Jaccard_Similarity(names_clusters_kmeans[i], names_clusters_hierarchical[j])
    plt.figure(figsize=(10, 10))
    sns.heatmap(jaccard_matrix, annot=True, xticklabels=[f"Cluster {i+1}" for i in range(m)], yticklabels=[f"Cluster {i+1}" for i in range(n)], cmap="YlGnBu")
    plt.xlabel("Hierarchical Clustering")
    plt.ylabel("K-Means Clustering")
    plt.title("Jaccard Similarity between K-Means and Hierarchical Clustering")
    plt.savefig("../OUTPUT/jaccard_similarity.png")
    return jaccard_matrix

def Try_All_Set_Map_Jaccard(cluster1, cluster2):
    """
    Calculates the Jaccard similarity between two clusters by trying all possible set mappings.

    Args:
        cluster1 (list): The first cluster.
        cluster2 (list): The second cluster.

    Returns:
        tuple: A tuple containing the following:
            - best_avg_jaccard (float): The highest average Jaccard similarity score.
            - jaccard_all (list): A list of Jaccard similarity scores for each pair of clusters.
            - best_permutation (list): The best permutation of indices that maximizes the average Jaccard similarity.

    Raises:
        ValueError: If the clusters are not of the same size.

    """
    if len(cluster1) != len(cluster2):
        print(
            f"Clusters are not of same size\nCluster 1: {len(cluster1)}\nCluster 2: {len(cluster2)}"
        )
        raise ValueError("Clusters are not of same size")

    n = len(cluster1)
    best_avg_jaccard = -1
    best_permutation = []
    permutations = generate_permutations(n)
    for permutation in permutations:
        set_map = {}
        for i in range(n):
            set_map[i] = permutation[i]
        new_cluster1 = [set_map[i] for i in range(n)]
        avg_jaccard = 0
        for i in range(n):
            avg_jaccard += Jaccard_Similarity(cluster1[i], cluster2[new_cluster1[i]])
        avg_jaccard /= n
        if avg_jaccard > best_avg_jaccard:
            best_avg_jaccard = avg_jaccard
            best_permutation = permutation
    jaccard_all = []
    for i in range(n):
        jaccard_all.append(
            Jaccard_Similarity(cluster1[i], cluster2[best_permutation[i]])
        )
    return best_avg_jaccard, jaccard_all, best_permutation


def write_cluster_index_to_file(cluster, dataset, file_path):
    """
    Write the cluster index to a file.

    Args:
        cluster (list): A list of lists containing the names of countries in each cluster.
        dataset (list): A list of entries, where each entry is a list of features.
        file_path (str): The path to the file where the cluster index will be written.

    Returns:
        None
    """
    n = len(cluster)
    name_map = {}
    i = 0
    for entry in dataset:
        name_map[entry[0]] = i
        i += 1
    with open(file_path, "w") as f:
        for j in range(n):
            print_string = ""
            i = 0
            for name in cluster[j]:
                if i != 0:
                    print_string += ", "
                print_string += str(name_map[name])
                i += 1
            f.write(f"{print_string}\n")


if __name__ == "__main__":
    cur_time = time.time()
    start_time = time.time()
    print("=====================================")
    print(
        "Country Grouping using Complete Linkage Divisive (Top-Down) Clustering Technique"
    )
    print("Student Name: Bratin Mondal\nStudent ID: 21CS10016")
    print("=====================================")
    print("Loading Data...")
    print("=====================================")
    attributes, entries = DataLoader()
    print("Data Loaded Successfully")
    print("=====================================")
    print("Time Taken to Load Data: ", time.time() - cur_time)
    print("=====================================")
    print("Finding Best K using K-Means Clustering")
    cur_time = time.time()
    best_k = find_best_k(entries)
    print("=====================================")
    print("Best K: ", best_k)
    print("Time Taken to Find Best K: ", time.time() - cur_time)
    print("=====================================")
    print("Clustering with Hierarchical Clustering")
    print("=====================================")
    cur_time = time.time()
    names_clusters, clusters, centroids = Hierarchical_Divisve_Clustering(
        entries, number_of_clusters=best_k
    )
    write_clusters_to_file(
        names_clusters, clusters, centroids, "../OUTPUT/cluster_output_hierarchical.txt"
    )
    silhouette = calculate_Silhouette(clusters)
    print(
        "Time Taken to Cluster using Hierarchical Clustering: ", time.time() - cur_time
    )
    print("=====================================")
    print("Silhouette: ", silhouette)
    print("Cluster Output File: cluster_output_hierarchical.txt")
    print("=====================================")
    print("Clustering using K-Means and Hierarchical Clustering completed")
    print("=====================================")
    print("Jaccard Similarity between K-Means and Hierarchical Clustering")
    names_clusters_kmeans, clusters_kmeans = read_clusters_from_file(
        "../OUTPUT/cluster_output_{}.txt".format(best_k)
    )
    names_clusters_hierarchical, clusters_hierarchical = read_clusters_from_file(
        "../OUTPUT/cluster_output_hierarchical.txt"
    )
    cur_time = time.time()
    avg_jaccard, jaccard_all, best_permutation = Try_All_Set_Map_Jaccard(
        names_clusters_kmeans, names_clusters_hierarchical
    )
    print("=====================================")
    print("Time Taken to Calculate Jaccard Similarity: ", time.time() - cur_time)
    Jaccard_Similarity_Visualization(names_clusters_kmeans, names_clusters_hierarchical)
    for i in range(best_k):
        print("=====================================")
        print(
            f"Cluster {i+1} of K-Means is mapped to Cluster {best_permutation[i]+1} of Hierarchical Clustering"
        )
        print(f"Jaccard Similarity: {jaccard_all[i]}")
    print("=====================================")
    print("Writing Cluster Index to File")
    write_cluster_index_to_file(
        names_clusters_kmeans, entries, "../SUBMISSION/kmeans.txt"
    )
    write_cluster_index_to_file(
        names_clusters_hierarchical, entries, "../SUBMISSION/divisive.txt"
    )
    print("=====================================")
    print("Cluster Index Written to File")
    print("=====================================")
    print("Total Time Taken: ", time.time() - start_time)
