// Copyright 2016, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CPP_INTERFACE_H_
#define SRC_ARTM_CPP_INTERFACE_H_

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "artm/messages.pb.h"
#include "artm/c_interface.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);   \
  void operator=(const TypeName&)

#ifndef ARTM_ERROR_CODES_EXIST
#define ARTM_ERROR_CODES_EXIST
enum ArtmErrorCodes {
  ARTM_SUCCESS = 0,                   // Has no corresponding exception type.
  ARTM_STILL_WORKING = -1,            // Has no corresponding exception type.
  ARTM_INTERNAL_ERROR = -2,
  ARTM_ARGUMENT_OUT_OF_RANGE = -3,
  ARTM_INVALID_MASTER_ID = -4,
  ARTM_CORRUPTED_MESSAGE = -5,
  ARTM_INVALID_OPERATION = -6,
  ARTM_DISK_READ_ERROR = -7,
  ARTM_DISK_WRITE_ERROR = -8,
};
#endif

namespace artm {

// Exception handling in cpp_interface
#define DEFINE_EXCEPTION_TYPE(Type, BaseType)                  \
class Type : public BaseType { public:  /*NOLINT*/             \
  Type() : BaseType("") {}                                     \
  explicit Type(std::string message) : BaseType(message) {}    \
};

DEFINE_EXCEPTION_TYPE(InternalError, std::runtime_error);
DEFINE_EXCEPTION_TYPE(ArgumentOutOfRangeException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(InvalidMasterIdException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(CorruptedMessageException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(InvalidOperationException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(DiskReadException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(DiskWriteException, std::runtime_error);

int HandleErrorCode(int artm_error_code);

#undef DEFINE_EXCEPTION_TYPE

void ParseCollection(const CollectionParserConfig& config);
void ConfigureLogging(const ConfigureLoggingArgs& args);

class Matrix {
 public:
  Matrix();
  explicit Matrix(int no_rows, int no_columns);

  void resize(int no_rows, int no_columns);

  int no_rows() const;
  int no_columns() const;

  float& operator() (int index_row, int index_col);
  const float& operator() (int index_row, int index_col) const;

  float* get_data();
  const float* get_data() const;

 private:
  int no_rows_;
  int no_columns_;
  std::vector<float> data_;
  DISALLOW_COPY_AND_ASSIGN(Matrix);
};

class MasterModel {
 public:
  explicit MasterModel(const MasterModelConfig& config);
  explicit MasterModel(int id);
  ~MasterModel();

  int id() const { return id_; }
  MasterComponentInfo info() const;  // misc. diagnostics information

  MasterModelConfig config() const;
  void Reconfigure(const MasterModelConfig& config);

  // Operations to work with dictionary through disk
  void GatherDictionary(const GatherDictionaryArgs& args);
  void FilterDictionary(const FilterDictionaryArgs& args);
  void ImportDictionary(const ImportDictionaryArgs& args);
  void ExportDictionary(const ExportDictionaryArgs& args);
  void DisposeDictionary(const std::string& dictionary_name);

  // Operations to work with dictinoary through memory
  void CreateDictionary(const DictionaryData& args);
  DictionaryData GetDictionary(const GetDictionaryArgs& args);

  // Operatinos to work with batches through memory
  void ImportBatches(const ImportBatchesArgs& args);
  void DisposeBatch(const std::string& batch_name);

  // Operations to work with model
  void InitializeModel(const InitializeModelArgs& args);
  void OverwriteModel(const TopicModel& args);
  void ImportModel(const ImportModelArgs& args);
  void ExportModel(const ExportModelArgs& args);
  void FitOnlineModel(const FitOnlineMasterModelArgs& args);
  void FitOfflineModel(const FitOfflineMasterModelArgs& args);
  void DisposeModel(const std::string& model_name);

  // Apply model to batches
  ThetaMatrix Transform(const TransformMasterModelArgs& args);
  ThetaMatrix Transform(const TransformMasterModelArgs& args, Matrix* matrix);

  // Retrieve operations
  TopicModel GetTopicModel();
  TopicModel GetTopicModel(const GetTopicModelArgs& args);
  TopicModel GetTopicModel(Matrix* matrix);
  TopicModel GetTopicModel(const GetTopicModelArgs& args, Matrix* matrix);
  ThetaMatrix GetThetaMatrix();
  ThetaMatrix GetThetaMatrix(const GetThetaMatrixArgs& args);
  ThetaMatrix GetThetaMatrix(Matrix* matrix);
  ThetaMatrix GetThetaMatrix(const GetThetaMatrixArgs& args, Matrix* matrix);

  // Retrieve scores
  ScoreData GetScore(const GetScoreValueArgs& args);
  template <typename T>
  T GetScoreAs(const GetScoreValueArgs& args);

 private:
  int id_;
  bool is_weak_ref_;
  MasterModel(const MasterModel& rhs);
  MasterModelConfig& operator=(const MasterModel&);
};

template <typename T>
T MasterModel::GetScoreAs(const GetScoreValueArgs& args) {
  auto score_data = GetScore(args);
  T score;
  score.ParseFromString(score_data.data());
  return score;
}

}  // namespace artm

#endif  // SRC_ARTM_CPP_INTERFACE_H_
