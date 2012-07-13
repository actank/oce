'rules': [
        {
          'rule_name': 'genproto',
          'extension': 'proto',
          'inputs': [
            '<(PRODUCT_DIR)/<(EXECUTABLE_PREFIX)protoc<(EXECUTABLE_SUFFIX)',
          ],
          'variables': {
            # The protoc compiler requires a proto_path argument with the
            # directory containing the .proto file.
            # There's no generator variable that corresponds to this, so fake it.
            'rule_input_relpath': 'browser/userfeedback/proto',
          },
          'outputs': [
            '<(protoc_out_dir)/chrome/<(rule_input_relpath)/<(RULE_INPUT_ROOT).pb.h',
            '<(protoc_out_dir)/chrome/<(rule_input_relpath)/<(RULE_INPUT_ROOT).pb.cc',
          ],
          'action': [
            '<(PRODUCT_DIR)/<(EXECUTABLE_PREFIX)protoc<(EXECUTABLE_SUFFIX)',
            '--proto_path=./<(rule_input_relpath)',
            './<(rule_input_relpath)/<(RULE_INPUT_ROOT)<(RULE_INPUT_EXT)',
            '--cpp_out=<(protoc_out_dir)/chrome/<(rule_input_relpath)',
          ],
          'message': 'Generating C++ code from <(RULE_INPUT_PATH)',
        },
      ],
