# Specification of the core skeleton of QNX resource manager low level design
This document describes the low level design of the core skeleton of QNX resource manager which intended to be used as ASIL B library serving the need of the platform described in [Specification of the hardware register resource managers architecture design](../../../../../../pas/hw_reg_res_mgrs/doc/score/static_reflection_with_serialization/README.md)

## Introduction
In general terms, a resource manager is a process that registers a name in the filesystem name space. Other processes use that path to communicate with the resource manager.
QNX allows user-written processes to act as resource managers that can be started and stopped dynamically

There is a fixed recommended sequence should be followed to start, setup and attach a resource manager process mentioned in [QNX Resource Manager concept](https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.sys_arch/topic/resource.html), considering the limitation and constraints documented in [Ticket-25109](broken_link_j/Ticket-25109) comments section.

## Design choice

The intention of this library is to provide a common implementation for constructing a resource manager within an application. It follows the initialization sequence recommended by QNX documentation, implemented by the ResourceManager class, and allows user-specific configuration through the use of the Resource class, where users can configure filepaths, callbacks, and other features of the "resources" that their ResourceManager should provide and manage.

The Core Design was divided into two main parts:
- The Fixed sequence recommended by QNX system to setup and initialize the resource manager which is encapsulated in the `ResourceManager` class.
- The configuration part, (`Resource` class)configuration for each resource to be managed by the resource manager (ex: resource path , resource handlers ..) or (`ResourceManagerConfig` struct) configuration for the resource manager process generally (message size, permissions ...).
    - The two types of the configuration should differ each use case and be passed to the base class (`ResourceManager`) as constructor arguments by the parent application.


Note: There is a reason to not inherit the `Application class` in the lifecycle component. The `application class` have a similar public methods for `Run()` and `Initialize()`. However, a component-specific setup logic is required before calling `ResourceManager::Initialize()`, So there is no value is gained by inheritance over composition.

*expected that Component-specific Application will contain a ResourceManager* [see Usage](#usage)

## Static Architecture
The below component diagrams show the interaction between the user of the base template class and the resource manager skeleton:

![Resource Manager Core template class diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/os/utils/qnx/resource_manager/detailed_design/ResourceManager_ClassDiagram.puml)

![Resource Manager Core component diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/os/utils/qnx/resource_manager/detailed_design/ResourceManager_ComponentDiagram.puml)

### Functional interfacing

As shown in the diagrams:
- we have the the skeleton of QNX resource manager encapsulated in base class which is called `ResourceManager`.
- The core class `ResourceManager` delegates some specific configuration and provide more flexibility to each resource manger to configure and handle its own devices independently, as following :
    - `ResourceManagerConfig` struct should be given in constructing the `ResourceManager()`.
    - At least one defined `Resource` Object or more should be given in constructing the `ResourceManager()`.
- The entry application that would launch the resource manager should just instantiate the resource manager class and invoke ResourceManager::Initialize()
- `ResourceManager::Initialize()` is following the meant sequence to setup and attach the resource manager to the system by doing the following :
  - Initialize the dispatch interface.
  - Initialize the callback/handlers of the POSIX functions like (read,write,open..) for each `Resource`.
   *Note : to provide a generic way and delegating the responsibility of handling the POSIX requests to the user application, an abstract/interface class for each request is implemented providing virtual handler for the request to be implemented by the user, those classes is mapped to the requests and registered on the system via static c functions implemented in the same context of the base class `ResourceManager` and passed to the system*

  - Initialize the iofunc parameters for each `Resource`.
  - Attach each `Resource` with it's specific path.
  - Allocate the context structure of the resource manager.
- `ResourceManager::Run()` Containing the super loop of the resource manger go to ReceiverBlocked state till receiving any request. and once it receives the request dispatch the request to the right registered handler. then go again to ReceiverBlocked state and so.
### Dependencies

The core implementation of the resource manager has the following dependencies which should be provided:
- score::os (QNX APIs by OSAL) interface with resmgr library and procnto APIS need to attach the resource manager.
   - score/os/qnx:iofunc
   - score/os/qnx:resmgr
   - score/os/qnx:types
   - score/os/qnx:dispatch
   - platform/aas/lib/os:errno
   - platform/aas/lib/result
- BMW::Middleware Logging library to monitor and notify the system with a proper state for the resource manager.
    - platform/aas/ara/log
- QNX Standard Libraries for c/c++.

## Dynamic Architecture
The following diagram represents behavior of the resource manager during the Runtime:

![Resource Manager Core startup sequence diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/os/utils/qnx/resource_manager/detailed_design/ResourceManager_StartupSequenceDiagram.puml)

![Resource Manager Core runtime sequence diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/os/utils/qnx/resource_manager/detailed_design/ResourceManager_RuntimeSequenceDiagram.puml)

![Resource Manager Core activity diagram - Initialize](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/os/utils/qnx/resource_manager/detailed_design/ResourceManager_ActivityDiagram_Initialize.puml)

![Resource Manager Core activity diagram - Run](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/os/utils/qnx/resource_manager/detailed_design/ResourceManager_ActivityDiagram_Run.puml)

## Performance
### Limitations and Constraints
- The ResourceManager class initially would support just single-threaded resource manager.
- The process running the resource manager should have `PROCMGR_AID_PUBLIC_CHANNEL` ability flag enabled to create public channel.
- System structures like `dispatch_t` in QNX libraries structures is opaque and shouldn't be accessed directly without calling the specific and suitable system API.
- A resource manager is uniquely identified by a node descriptor, process ID, and a channel ID. The process manager's table entry associates the resource manager with a name, a handle (to distinguish multiple names when a resource manager registers more than one name), and an open type.
- Resource manager process should be started as a root in order to attach the pathname space, but it's a good idea to use `procmgr_ability()` to retain the abilities that the resource manager needs, and then run as a non-root user.
- If the process of the resource manager is critical one, a Resource constraint thresholds should be activated using `RESMGR_FLAG_RCM` flag for the attributes. [see this](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.prog/topic/process_resource_constraint.html).
- When the client's library issue the query call, the process manager looks through all of its tables for any registered pathname prefixes that match the name. If another resource manager had previously registered the name, more than one match would be found. So, in this case, The process manager will reply to the open() with the list of matched servers or resource managers. The servers are queried in turn about their handling of the path, with the longest match being asked first.
- The ResourceManager class initially would support device resource manager rather than filesystem resource manager, the functionality would be extended upon needs.

## Testing approach
In general, Unit testing with GTest environment should test and covers all methods and functionality provided by the core skeleton implementation of the resource manger.
## Assumption of use
### When to use the resource manager ?
- if you need to handle any resource or device with POSIX based interfacing.
- if the program is event-driven based architecture. the native IPC mechanisms of QNX is highly compatible with less documentation and implementation needed.
-If you need to provide a high degree of abstraction from the QNX system and HW interacting.
- An extra need to the resource manager comes from the facility to interact with the command line utilities.
### When not to use the resource manager ?
- If the device driver or the resource handler does not need to receive messages.
-  if the launcher process interfaces only with its children. The parent has access to all the children's information required to interface with them.
-  If you must transfer high volumes of data at a very high rate, a resource manager can be a problem. Since resource managers basically interface via IPC kernel primitives over the native synchronous message passing mechanisms.

## Usage
### How to use the resource manager ?

    1- Construct `Resource` class with right resource configuration based on the needs.
    2- Construct `ResourceManagerConfig` struct with the right resource manager configuration.
    3- Instantiate `ResourceManager` class taking both of `Resource` and `ResourceManagerConfig` as parameters to the constructor.
    4- Initialize().
    5- Run().
    6- From the client side :
        - The interfacing with the resource manager should be handled as the normal interacting with the files in the system using the POSIX interfaces.

## Summary
As a summary the Core implementation of QNX resource manager is encapsulated in ResourceManager class providing level of flexibility and configurability to be used in different contexts and handle different kind of resource.
