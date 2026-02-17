# Dependency graph

The dependency graph below shows the separation of the frontend from the
implementation details of the backends. Applying the component principles from
[1] the goal here is to structure the dependencies in a way that instable
components depend on stable components.

![Use Case Diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/mw/log/design/frontend_dependency_graph.uxf)

The `mw::log` component is divided in a stable frontend and the backends with
the implementation details. The frontend shall be API compatible to the ara::log
standard 20-11 for the `Logger` and `LogStream`. In addition, the frontend
contains the interfaces `IRecorder` and `IRecorderFactory` to abstract the
backend. The backend implementation will depend on the frontend and implement
the interfaces and the concrete recorder factory.

Important is that the component boundaries between frontend and details are only
crossed **one-way**. Thus the frontend component must **not** depend on the details
component, and should only contain the essential entities that are needed for
the public API.

In order to instantiate the concrete recorder factory, the `Runtime` depends on
the `RecorderFactory`. This would violate the one-way principle from above.
This is tackled by declaring the `CreateRecorderFactory()` function in the `IRecorderFactory` interface and only supply the implementation
of that in the details component.

# References

[1] Martin, Robert C., et al. Clean architecture: a craftsman's guide to
software structure and design. No. s 31. Prentice Hall, 2018.
