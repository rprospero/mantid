// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2012 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_API_BATCHALGORITHMRUNNER_H_
#define MANTID_API_BATCHALGORITHMRUNNER_H_

#include "DllOption.h"
#include "MantidAPI/Algorithm.h"

#include <QObject>

#include <Poco/ActiveMethod.h>
#include <Poco/ActiveResult.h>
#include <Poco/NObserver.h>
#include <Poco/Void.h>

#include <QMetaType>
#include <deque>
#include <mutex>

namespace MantidQt {
namespace API {
class EXPORT_OPT_MANTIDQT_COMMON ConfiguredAlgorithm {
public:
  using AlgorithmRuntimeProps = std::map<std::string, std::string>;

  ConfiguredAlgorithm(Mantid::API::IAlgorithm_sptr algorithm,
                      AlgorithmRuntimeProps properties);
  virtual ~ConfiguredAlgorithm();

  virtual Mantid::API::IAlgorithm_sptr algorithm() const;
  virtual AlgorithmRuntimeProps properties() const;

protected:
  Mantid::API::IAlgorithm_sptr m_algorithm;

private:
  AlgorithmRuntimeProps m_properties;
};

using ConfiguredAlgorithm_sptr = boost::shared_ptr<ConfiguredAlgorithm>;

class BatchCompleteNotification : public Poco::Notification {
public:
  BatchCompleteNotification(bool inProgress, bool error)
      : Poco::Notification(), m_inProgress(inProgress), m_error(error) {}

  bool isInProgress() const { return m_inProgress; }
  bool hasError() const { return m_error; }

private:
  bool m_inProgress;
  bool m_error;
};

class BatchCancelledNotification : public Poco::Notification {
public:
  BatchCancelledNotification() : Poco::Notification() {}
};

class AlgorithmCompleteNotification : public Poco::Notification {
public:
  AlgorithmCompleteNotification(ConfiguredAlgorithm_sptr algorithm)
      : Poco::Notification(), m_algorithm(algorithm) {}

  ConfiguredAlgorithm_sptr algorithm() const { return m_algorithm; }

private:
  ConfiguredAlgorithm_sptr m_algorithm;
};

class AlgorithmStartedNotification : public Poco::Notification {
public:
  AlgorithmStartedNotification(ConfiguredAlgorithm_sptr algorithm)
      : Poco::Notification(), m_algorithm(algorithm) {}

  ConfiguredAlgorithm_sptr algorithm() const { return m_algorithm; }

private:
  ConfiguredAlgorithm_sptr m_algorithm;
};

class AlgorithmErrorNotification : public Poco::Notification {
public:
  AlgorithmErrorNotification(ConfiguredAlgorithm_sptr algorithm,
                             std::string const &errorMessage)
      : Poco::Notification(), m_algorithm(algorithm),
        m_errorMessage(errorMessage) {}

  ConfiguredAlgorithm_sptr algorithm() const { return m_algorithm; }
  std::string errorMessage() const { return m_errorMessage; }

private:
  ConfiguredAlgorithm_sptr m_algorithm;
  std::string m_errorMessage;
};

/**
 * Algorithm runner for execution of a queue of algorithms

 @date 2014-08-10
*/

class EXPORT_OPT_MANTIDQT_COMMON BatchAlgorithmRunner : public QObject {
  Q_OBJECT

public:
  using AlgorithmRuntimeProps = std::map<std::string, std::string>;

  explicit BatchAlgorithmRunner(QObject *parent = nullptr);
  ~BatchAlgorithmRunner() override;

  /// Adds an algorithm to the execution queue
  void addAlgorithm(Mantid::API::IAlgorithm_sptr algo,
                    AlgorithmRuntimeProps props = AlgorithmRuntimeProps());
  void setQueue(std::deque<ConfiguredAlgorithm_sptr> algorithm);
  /// Clears all algorithms from queue
  void clearQueue();
  /// Gets size of queue
  size_t queueLength();

  /// Executes the batch, waits for the result and returns it
  bool executeBatch();
  /// Starts the batch executing and returns immediately
  void executeBatchAsync();
  /// Request to cancel processing the batch
  void cancelBatch();

  /// Sets if the execuion should be stopped if an error is detected
  void stopOnFailure(bool stopOnFailure);

signals:
  /// Emitted when a batch has finished executing
  void batchComplete(bool error);
  void batchCancelled();
  void algorithmStarted(MantidQt::API::ConfiguredAlgorithm_sptr algorithm);
  void algorithmComplete(MantidQt::API::ConfiguredAlgorithm_sptr algorithm);
  void algorithmError(MantidQt::API::ConfiguredAlgorithm_sptr algorithm,
                      std::string errorMessage);

private:
  /// Implementation of algorithm runner
  bool executeBatchAsyncImpl(const Poco::Void &);
  /// Sets up and executes an algorithm
  bool executeAlgo(ConfiguredAlgorithm_sptr algorithm);

  /// Handlers for notifications
  void handleBatchComplete(const Poco::AutoPtr<BatchCompleteNotification> &pNf);
  void
  handleBatchCancelled(const Poco::AutoPtr<BatchCancelledNotification> &pNf);
  void handleAlgorithmStarted(
      const Poco::AutoPtr<AlgorithmStartedNotification> &pNf);
  void handleAlgorithmComplete(
      const Poco::AutoPtr<AlgorithmCompleteNotification> &pNf);
  void
  handleAlgorithmError(const Poco::AutoPtr<AlgorithmErrorNotification> &pNf);

  /// The queue of algorithms to be executed
  std::deque<ConfiguredAlgorithm_sptr> m_algorithms;

  /// The current algorithm being executed
  Mantid::API::IAlgorithm_sptr m_currentAlgorithm;

  /// If execution should be stopped on algorithm failure
  bool m_stopOnFailure;

  /// User has requested to cancel processing
  bool m_cancelRequested;
  std::mutex m_mutex;
  void resetState();
  bool cancelRequested();

  /// Notification center used to handle notifications from active method
  mutable Poco::NotificationCenter m_notificationCenter;
  /// Observer for notifications
  Poco::NObserver<BatchAlgorithmRunner, BatchCompleteNotification>
      m_batchCompleteObserver;
  Poco::NObserver<BatchAlgorithmRunner, BatchCancelledNotification>
      m_batchCancelledObserver;
  Poco::NObserver<BatchAlgorithmRunner, AlgorithmStartedNotification>
      m_algorithmStartedObserver;
  Poco::NObserver<BatchAlgorithmRunner, AlgorithmCompleteNotification>
      m_algorithmCompleteObserver;
  Poco::NObserver<BatchAlgorithmRunner, AlgorithmErrorNotification>
      m_algorithmErrorObserver;

  /// Active method to run batch runner on separate thread
  Poco::ActiveMethod<bool, Poco::Void, BatchAlgorithmRunner,
                     Poco::ActiveStarter<BatchAlgorithmRunner>>
      m_executeAsync;
  /// Holds result of async execution
  Poco::ActiveResult<bool> executeAsync();

  void addAllObservers();
  void removeAllObservers();
};
} // namespace API
} // namespace MantidQt

#endif /* MANTID_API_BATCHALGORITHMRUNNER_H_ */
