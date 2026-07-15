Assumptions of Use
##################

.. aou_req:: Undocumented or Private APIs
   :id: aou_req__log__private_or_undocumented_apis
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :status: valid

   Undocumented or private API from ``mw::log`` **shall not** be used.

   The public API of ``mw::log`` is contained in the ``mw::log``
   namespace and is annotated with the documentation tag ``\public``.
   Everything else is considered part of the private API and **shall not** be
   used.
   Entities from any implementation ``details`` namespace **shall not** be
   used.

   In the following example, the ``TextRecorder`` class is part of a detail
   namespace and **shall not** be used directly. It may be used indirectly but
   only though public API:

   .. code:: cpp

      namespace mw::log
      {
      namespace details
      {

      class TextRecorder : public Recorder // Private undocumented API SHALL NOT be used.
      {
          // ...
      };

      } // namespace detail

      /*
      * \public
      */
      LogStream LogInfo(); // Public API is OK to be used by Safety-Related Application Software.

      } // namespace mw::log


.. aou_req:: Unsupported Contexts
   :id: aou_req__log__unsupported_contexts
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :status: valid

   The log frontend API **shall not** be used from unsupported contexts.

   1. Threads: mw::log API documented and tagged ``\thread-safe`` shall be
           thread safe.

   2. Signal handler: mw::log API **shall not** be called from signal handlers.

   3. Interrupt handler: mw::log API **shall not** be called from interrupt
           handlers.

   The majority of the mw::log API is thread-safe. Thread-safe API can be
   recognized by the `\thread-safe` tag in the documentation:

   .. code:: cpp

      class Logger
      {
          /// \brief Creates a LogStream to log messages of criticality `Fatal` (highest).
          ///
          /// \public
          /// \thread-safe
          ///
          /// \details Fatal shall be used on errors that cannot be recovered and will lead to an overall failure in the
          /// system. The message will be logged under the context that was provided on construction.
          ///
          /// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
          log::LogStream LogFatal() const noexcept;
      };

   ``LogStream`` classes are **not** thread-safe. For example, the following
   code shows what **not** to do with ``LogStream`` classes:

   .. code:: cpp

      auto log_stream = logger.LogInfo();

      std::thread t1([&](){
          // Do not access log_stream from another thread!
          log_stream << "test";
      });

      log_stream << "hello";


.. aou_req:: Logging During the C++ Static Storage
   construction and destruction.
   :id: aou_req__log__no_static_log
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :status: valid

    mw::log SHALL NOT be used during the C++ static storage

    Safety-Related Platform Software and Safety-Related Application Software
    **shall not** use mw::log during C++ static storage initialization and
    destruction.

   .. code:: cpp


      struct MyClass {
          MyClass() {
              mw::log::LogInfo() << "MyClass()"; // mw::log SHALL NOT be used before entering main().
          }
          ~MyClass() {
              mw::log::LogInfo() << "MyClass()"; // mw::log SHALL NOT be used after exiting main().
          }
      };


      static MyClass singleton{};

.. aou_req:: Console Logging
   :id: aou_req__log__console_logging
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :status: valid

   Console logging **shall not** be used in production.


.. aou_req:: Applications relying on logs
   verification/qualification/case.
   :id: aou_req__log__relying_on_logs
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :status: valid

   Applications **shall not** rely on mw::log output for safety
   verification/qualification/case.

   Safety-Related Platform Software and Safety-Related Application Software
   **shall not** rely on the presence of any form of mw::log output for delivering
   their business logic.
   This shall hold even for logs sent out by the application itself.

   Safety-Related Platform Software and Safety-Related Application Software
   behavior **shall not** depend on log messages:

   .. code:: cpp

      void Step() {
          // Do not rely on log messages!
          if ( ReceiveStopLogMessage() ) {

              ExecuteStopCommand();

          }
          // ...

   This requirement shall be verified for each assigned application. The whole
   dependency tree of **all** included libraries shall be also verified on this
   requirement in the application use case and by application developers.
