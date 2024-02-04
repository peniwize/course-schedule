# Start of "solution.py".

from collections import deque
import inspect
import time
from typing import List
from typing import Optional
from typing import Set

"""
    There are a total of numCourses courses you have to take, labeled from 
    0 to numCourses - 1. You are given an array prerequisites where 
    prerequisites[i] = [ai, bi] indicates that you must take course bi first 
    if you want to take course ai.

        * For example, the pair [0, 1], indicates that to take course 0 you 
          have to first take course 1.
    
    Return true if you can finish all courses. Otherwise, return false.

    Constraints:

        * 1 <= numCourses <= 2000
        * 0 <= prerequisites.length <= 5000
        * prerequisites[i].length == 2
        * 0 <= ai, bi < numCourses
        * All the pairs prerequisites[i] are unique.
"""

class Solution1_DFS:
    """
        The prerequisites create a dependency tree.
        Return true only when all branches in the tree contain no cycles.
        
        Populate a dictionary with all courses.  Keys are prerequisites[0],
        values are arrays composed of prerequisites[1], which handles 
        courses with more than one prerequisite.  Use a recursive depth
        first search to visit each individual branch of the prerequisite
        tree.  Remove the branch (or sub-branch) from the tree after 
        visiting it (DP).  As each node in the branch is visited, check
        to see if it was already visited.  If it was then there is a cyclic
        dependency in the branch, which results in false being returned.
        Add branch nodes to the visited collection as the branch is traversed
        from root to tip and remove the added nodes as the stack unwinds back
        from tip to root (backtracking).

        Time = O(n)
               n = number of prerequisites.  Prerequisite tree branch nodes
                   are removed from the prerequisite tree after being visited,
                   so the maximum number of nodes that can ever possibly be 
                   visited is n.

        Space = O(n)
                n = maximum call stack depth when all prerequisites reference
                    each other and create a linear chain.
    """
    def canFinish(self, numCourses: int, prerequisites: List[List[int]]) -> bool:
        courses = {}
        for course, dependency in prerequisites:
            if course in courses:
                courses[course].append(dependency)
            else:
                courses[course] = [dependency]

        def helper(courses, course):
            if course in visited:
                return False
            if course not in courses:
                return True
            visited.add(course)
            for dependency in courses[course]:
                if not helper(courses, dependency):
                    return False
            visited.discard(course)
            del courses[course]
            return True
        
        visited = set()
        while 0 < len(courses):
            course = next(iter(courses))
            if not helper(courses, course):
                return False
        
        return True

class Solution2_TopologicalSort:
    """
        The prerequisites create a dependency tree.
        Return true only when all branches in the tree contain no cycles.
        
        Use a topological sort to detect cycles.
          1) Create an adjacency list from the prerequisites.
          2) Count the number of edges coming in to a node (it's in-degree).
          3) Push the nodes with an in-degree of zero (0) on to the que.
          4) While the queue is not empty:
        4.1)   Pop node ('A') off of que.
        4.2)   Decrement the in-degree of each node referenced by 'A', i.e.
               each node that can be reached by following an edge from 'A'.
        4.3)   If the in-degree of a node becomes zero (0), push the node 
               on to the que.
          5) If the number of nodes pushed to or popped from the que is equal
             to the number of nodes in the graph then there are no cycles in
             the dependency tree and the result is true.

        Time = O(V + V + E + V + V) => O(4V + E) => O(V+E)
               V = vertex count == numCourses
               E = edge count == len(prerequisites)
               Term 1: inDegrees allocation.
               Term 2: graph allocation.
               Term 3: graph population.
               Term 4: queue initialization.
               Term 5: queue processing.

        Space = O(V + V*V + V) => O(2V + V**2) => O(V**2)
                V = vertex count == numCourses
                Term 1: inDegrees capacity
                Term 2: graph capacity (all nodes could reference each other).
                Term 3: queue capacity (all nodes when none reference any other).
    """
    def canFinish(self, numCourses: int, prerequisites: List[List[int]]) -> bool:
        inDegrees = [0] * numCourses # In-degree's for each node.
        graph = [[] for _ in range(numCourses)];
        for edge in prerequisites:
            node = edge[0]
            tgt = edge[1]

            # Add node to graph.
            graph[node].append(tgt)
            
            # Increment in-degree for each outgoing edge from node.
            inDegrees[tgt] += 1
        
        que = deque()
        
        # Initialize queue with nodes that have a zero (0) in-degree.
        for node in range(len(inDegrees)):
            if 0 == inDegrees[node]:
                que.append(node)

        processCount = len(que) # Number of nodes processed by the queue.
        
        # Process the queue.
        while 0 < len(que):
            node = que.popleft()
            for tgt in graph[node]:
                inDegrees[tgt] -= 1
                if 0 == inDegrees[tgt]:
                    que.append(tgt)
                    processCount += 1
        
        return numCourses == processCount

def test1(solution):
    numCourses = 2
    prerequisites = [
        [1,0],
    ]
    expected = True
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

def test2(solution):
    numCourses = 2
    prerequisites = [
        [1,0],
        [0,1],
    ]
    expected = False
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

def test3(solution):
    numCourses = 5
    prerequisites = [
        [1,4],
        [2,4],
        [3,1],
        [3,2]
    ]
    expected = True
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

def test4(solution):
    numCourses = 8
    prerequisites = [
        [1,0],
        [2,6],
        [1,7],
        [6,4],
        [7,0],
        [0,5]
    ]
    expected = True
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

def test5(solution):
    numCourses = 20
    prerequisites = [
        [0,10],
        [3,18],
        [5,5],
        [6,11],
        [11,14],
        [13,1],
        [15,1],
        [17,4]
    ]
    expected = False
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

def test6(solution):
    numCourses = 3
    prerequisites = [
        [1,0],
        [2,0],
        [0,2]
    ]
    expected = False
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

def test7(solution):
    numCourses = 3
    prerequisites = [
        [1,0],
        [1,2],
        [0,1]
    ]
    expected = False
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

def test8(solution):
    numCourses = 100
    prerequisites = [
        [1,0],[2,0],[2,1],[3,1],[3,2],[4,2],[4,3],[5,3],[5,4],[6,4],[6,5],[7,5],[7,6],[8,6],[8,7],[9,7],[9,8],[10,8],[10,9],[11,9],[11,10],[12,10],[12,11],[13,11],[13,12],[14,12],[14,13],[15,13],[15,14],[16,14],[16,15],[17,15],[17,16],[18,16],[18,17],[19,17],[19,18],[20,18],[20,19],[21,19],[21,20],[22,20],[22,21],[23,21],[23,22],[24,22],[24,23],[25,23],[25,24],[26,24],[26,25],[27,25],[27,26],[28,26],[28,27],[29,27],[29,28],[30,28],[30,29],[31,29],[31,30],[32,30],[32,31],[33,31],[33,32],[34,32],[34,33],[35,33],[35,34],[36,34],[36,35],[37,35],[37,36],[38,36],[38,37],[39,37],[39,38],[40,38],[40,39],[41,39],[41,40],[42,40],[42,41],[43,41],[43,42],[44,42],[44,43],[45,43],[45,44],[46,44],[46,45],[47,45],[47,46],[48,46],[48,47],[49,47],[49,48],[50,48],[50,49],[51,49],[51,50],[52,50],[52,51],[53,51],[53,52],[54,52],[54,53],[55,53],[55,54],[56,54],[56,55],[57,55],[57,56],[58,56],[58,57],[59,57],[59,58],[60,58],[60,59],[61,59],[61,60],[62,60],[62,61],[63,61],[63,62],[64,62],[64,63],[65,63],[65,64],[66,64],[66,65],[67,65],[67,66],[68,66],[68,67],[69,67],[69,68],[70,68],[70,69],[71,69],[71,70],[72,70],[72,71],[73,71],[73,72],[74,72],[74,73],[75,73],[75,74],[76,74],[76,75],[77,75],[77,76],[78,76],[78,77],[79,77],[79,78],[80,78],[80,79],[81,79],[81,80],[82,80],[82,81],[83,81],[83,82],[84,82],[84,83],[85,83],[85,84],[86,84],[86,85],[87,85],[87,86],[88,86],[88,87],[89,87],[89,88],[90,88],[90,89],[91,89],[91,90],[92,90],[92,91],[93,91],[93,92],[94,92],[94,93],[95,93],[95,94],[96,94],[96,95],[97,95],[97,96],[98,96],[98,97],[99,97]
    ]
    expected = True
    startTime = time.time()
    result = solution.canFinish(numCourses, prerequisites)
    endTime = time.time()
    print("{}:{}({:.6f} sec) result = {}".format(inspect.currentframe().f_code.co_name, type(solution), endTime - startTime, result))
    assert(expected == result)

if "__main__" == __name__:
    test1(Solution1_DFS())
    test2(Solution1_DFS())
    test3(Solution1_DFS())
    test4(Solution1_DFS())
    test5(Solution1_DFS())
    test6(Solution1_DFS())
    test7(Solution1_DFS())
    test8(Solution1_DFS())

    test1(Solution2_TopologicalSort())
    test2(Solution2_TopologicalSort())
    test3(Solution2_TopologicalSort())
    test4(Solution2_TopologicalSort())
    test5(Solution2_TopologicalSort())
    test6(Solution2_TopologicalSort())
    test7(Solution2_TopologicalSort())
    test8(Solution2_TopologicalSort())

# End of "solution.py".
