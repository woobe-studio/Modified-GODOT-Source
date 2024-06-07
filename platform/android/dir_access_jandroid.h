/**************************************************************************/
/*  dir_access_jandroid.h                                                 */
/**************************************************************************/


#ifndef DIR_ACCESS_JANDROID_H
#define DIR_ACCESS_JANDROID_H

#include "core/os/dir_access.h"
#include "drivers/unix/dir_access_unix.h"
#include "java_godot_lib_jni.h"
#include <stdio.h>

/// Android implementation of the DirAccess interface used to provide access to
/// ACCESS_FILESYSTEM and ACCESS_RESOURCES directory resources.
/// The implementation use jni in order to comply with Android filesystem
/// access restriction.
class DirAccessJAndroid : public DirAccessUnix {
	static jobject dir_access_handler;
	static jclass cls;

	static jmethodID _dir_open;
	static jmethodID _dir_next;
	static jmethodID _dir_close;
	static jmethodID _dir_is_dir;
	static jmethodID _dir_exists;
	static jmethodID _file_exists;
	static jmethodID _get_drive_count;
	static jmethodID _get_drive;
	static jmethodID _make_dir;
	static jmethodID _get_space_left;
	static jmethodID _rename;
	static jmethodID _remove;
	static jmethodID _current_is_hidden;

public:
	virtual Error list_dir_begin() override; ///< This starts dir listing
	virtual String get_next() override;
	virtual bool current_is_dir() const override;
	virtual bool current_is_hidden() const override;
	virtual void list_dir_end() override; ///<

	virtual int get_drive_count() override;
	virtual String get_drive(int p_drive) override;

	virtual Error change_dir(String p_dir) override; ///< can be relative or absolute, return false on success
	virtual String get_current_dir() override; ///< return current dir location

	virtual bool file_exists(String p_file) override;
	virtual bool dir_exists(String p_dir) override;

	virtual Error make_dir(String p_dir) override;
	virtual Error make_dir_recursive(String p_dir) override;

	virtual Error rename(String p_from, String p_to) override;
	virtual Error remove(String p_name) override;

	virtual bool is_link(String p_file) override { return false; }
	virtual String read_link(String p_file) override { return p_file; }
	virtual Error create_link(String p_source, String p_target) override { return FAILED; }

	virtual uint64_t get_space_left() override;

	static void setup(jobject p_dir_access_handler);

	DirAccessJAndroid();
	~DirAccessJAndroid();

protected:
	String _get_root_string() const override;

private:
	int id = 0;

	int dir_open(String p_path);
	void dir_close(int p_id);
	String get_absolute_path(String p_path);
};

#endif // DIR_ACCESS_JANDROID_H
