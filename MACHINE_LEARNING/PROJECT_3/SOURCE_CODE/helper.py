import numpy as np
import pandas as pd
import time


def cosine_similarity(v1, v2):
    return np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))


def euclidean_distance(v1, v2):
    return np.linalg.norm(v1 - v2)


def DataLoader(file_path="../DATASET/country.csv"):
    data = pd.read_csv(file_path).dropna()
    attributes = data.columns
    entries = data.values
    return attributes, entries


def sort_clusters(names_clusters, clusters):
    sorted_clusters = []
    for i in range(len(names_clusters)):
        sorted_clusters.append([])
        for name in sorted(names_clusters[i]):
            index = names_clusters[i].index(name)
            sorted_clusters[i].append(clusters[i][index])
    sorted_names_clusters = [sorted(names) for names in names_clusters]
    return sorted_names_clusters, sorted_clusters


def K_Means_Clustering(k, entries, itr=20, random_seed=0):
    entry_without_country = [entry[1:] for entry in entries]
    names = [entry[0] for entry in entries]  # Extract country names
    np.random.seed(random_seed)
    centroids = np.array([entry_without_country[i] for i in np.random.choice(
        len(entry_without_country), k, replace=False)])

    for _ in range(itr):
        clusters = [[] for _ in range(k)]
        # Separate list to store country names in clusters
        names_clusters = [[] for _ in range(k)]
        for i, entry in enumerate(entry_without_country):
            similarities = [cosine_similarity(
                entry, centroid) for centroid in centroids]
            cluster_index = np.argmax(similarities)
            clusters[cluster_index].append(entry)
            # Append corresponding country name
            names_clusters[cluster_index].append(names[i])
        new_centroids = [np.mean(cluster, axis=0) for cluster in clusters]
        if np.array_equal(centroids, new_centroids):
            break
        centroids = new_centroids
    return names_clusters, clusters, centroids


def write_clusters_to_file(names_clusters, clusters, centroids, file_path):
    with open(file_path, "w") as f:
        f.write("Total Clusters: {}\n".format(len(clusters)))
        for i in range(len(clusters)):
            f.write("Cluster {}\n".format(i+1))
            f.write("Number of Countries: {}\n".format(len(clusters[i])))
            centroid_str = ' '.join(map(str, centroids[i]))
            f.write("Centroid: {}\n".format(centroid_str))

            for j in range(len(names_clusters[i])):
                f.write(names_clusters[i][j] + "\n")
                cluster_str = ' '.join(map(str, clusters[i][j]))
                cluster_str="Attributes: "+cluster_str
                f.write(cluster_str + "\n")
            f.write("\n")


def calculate_Silhouette(clusters, centroids):
    silhouette = 0
    sample_size = sum([len(cluster) for cluster in clusters])
    for i, cluster in enumerate(clusters):
        for entry in cluster:
            a = np.mean([euclidean_distance(entry, other)
                        for other in cluster])
            b = np.min([np.mean([euclidean_distance(entry, other) for other in other_cluster])
                       for j, other_cluster in enumerate(clusters) if j != i])
            silhouette += (b - a) / max(a, b)
    return (silhouette / sample_size)


def Hierarchical_Clustering(entries, k, linkage="complete"):
    entry_without_country = [entry[1:] for entry in entries]
    names = [entry[0] for entry in entries]  # Extract country names
    clusters = [[entry] for entry in entry_without_country]
    names_clusters = [[name] for name in names]
    while len(clusters) > k:
        max_similarity = -1
        for i in range(len(clusters)):
            for j in range(i+1, len(clusters)):
                if linkage == "complete":
                    similarity = np.min([cosine_similarity(
                        entry1, entry2) for entry1 in clusters[i] for entry2 in clusters[j]])
                elif linkage == "single":
                    similarity = np.max([cosine_similarity(
                        entry1, entry2) for entry1 in clusters[i] for entry2 in clusters[j]])
                elif linkage == "average":
                    similarity = np.mean([cosine_similarity(
                        entry1, entry2) for entry1 in clusters[i] for entry2 in clusters[j]])
                if similarity > max_similarity:
                    max_similarity = similarity
                    merge_index = (i, j)
        i, j = merge_index
        clusters[i].extend(clusters[j])
        names_clusters[i].extend(names_clusters[j])
        clusters.pop(j)
        names_clusters.pop(j)
    sorted_names_clusters, sorted_clusters = sort_clusters(
        names_clusters, clusters)
    mean_clusters = [np.mean(cluster, axis=0) for cluster in sorted_clusters]
    return sorted_names_clusters, sorted_clusters, mean_clusters


def find_best_k(entries):
    best_k = 0
    best_silhouette = -1
    for k in range(3, 7):
        print("=====================================")
        print("Clustering with K = ", k)
        names_clusters, clusters, centroids = K_Means_Clustering(
            k, entries, itr=20, random_seed=int(time.time()))
        write_clusters_to_file(
            names_clusters, clusters, centroids, "../OUTPUT/cluster_output_{}.txt".format(k))
        silhouette = calculate_Silhouette(clusters, centroids)
        print("=====================================")
        print("K: ", k, "\nSilhouette: ", silhouette)
        print(f"Cluster Output File: cluster_output_{k}.txt")
        print("=====================================")
        if silhouette > best_silhouette:
            best_silhouette = silhouette
            best_k = k
    return best_k


def read_clusters_from_file(file_path):
    with open(file_path, "r") as f:
        lines = f.readlines()
        num_clusters = int(lines[0].split(": ")[1])
        names_clusters = []
        clusters = []
        for i in range(1, num_clusters+1):
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
                clusters[cluster_index-1].append(cluster)
            else:
                name=lines[i].strip()
                names_clusters[cluster_index-1].append(name)
    return names_clusters, clusters
            

def Try_All_Set_Map_Jaccard(cluster1,cluster2):
    assert len(cluster1)==len(cluster2)
    n=len(cluster1)
    max_jaccard=-1
    for i in range(n):
        for j in range(n):
            set1=set(cluster1[i])
            set2=set(cluster2[j])
            jaccard=Jaccard_Similarity(set1,set2)
            if jaccard>max_jaccard:
                max_jaccard=jaccard


def Jaccard_Similarity(set1, set2):
    intersection = len(set1.intersection(set2))
    union = len(set1.union(set2))
    return intersection / union


if __name__ == "__main__":
    print("=====================================")
    print("Loading Data...")
    print("=====================================")
    attributes, entries = DataLoader()
    print("Data Loaded Successfully")
    best_k = find_best_k(entries)
    print("Best K: ", best_k)
    print("=====================================")
    print("Clustering with Hierarchical Clustering")
    print("=====================================")
    names_clusters, clusters, centroids = Hierarchical_Clustering(entries, best_k, linkage="complete")
    write_clusters_to_file(names_clusters, clusters, centroids, "../OUTPUT/cluster_output_hierarchical.txt")
    silhouette = calculate_Silhouette(clusters, centroids)
    print("=====================================")
    print("Silhouette: ", silhouette)
    print("Cluster Output File: cluster_output_hierarchical.txt")
    print("=====================================")
    print("Clustering Completed")
    print("=====================================")
    print("Jaccard Similarity between K-Means and Hierarchical Clustering")
    print("=====================================")
    names_clusters_kmeans, clusters_kmeans = read_clusters_from_file("../OUTPUT/cluster_output_{}.txt".format(best_k))
    names_clusters_hierarchical, clusters_hierarchical = read_clusters_from_file("../OUTPUT/cluster_output_hierarchical.txt")

