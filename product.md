# Vision
A tool to manage very large collections of objects in a simple and efficient manner.
Thread safety should be garantueed.
# Idea
Creating a wrapper for a STL container to make it thread-safe and easy to use.
# Requirements
- Key-Value pairs are stored in a STL map
- A shared mutex guards the map access
    - Shared locking is inteded for read access (e.g. at, size, ...)
    - Unique locking is intended for write operations (e.g. erase, clear, ...)
- Each Key-Value is assigned a shared mutex
    - Shared locking is intended for value read/write access (->)
    - Unique locking is intended for value erasure
- In order to avoid deadlocks:
    - Value member functions may only modify other key-value pairs in a hierarchical order, i.e., if we draw an access diagram of the values in the collection, there may be no cycle. Particularly, this implies that a member function is not allowed to erase itself from the collection.
    - The same rules applies for foreach callbacks.
# Architecture
# Implementation
# Tests
# Deployment
# Support