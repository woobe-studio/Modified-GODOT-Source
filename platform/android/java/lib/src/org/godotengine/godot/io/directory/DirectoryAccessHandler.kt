/**************************************************************************/
/*  DirectoryAccessHandler.kt                                             */
/**************************************************************************/


package org.godotengine.godot.io.directory

import android.content.Context
import android.util.Log
import org.godotengine.godot.io.directory.DirectoryAccessHandler.AccessType.ACCESS_FILESYSTEM
import org.godotengine.godot.io.directory.DirectoryAccessHandler.AccessType.ACCESS_RESOURCES

/**
 * Handles files and directories access and manipulation for the Android platform
 */
class DirectoryAccessHandler(context: Context) {

	companion object {
		private val TAG = DirectoryAccessHandler::class.java.simpleName

		internal const val INVALID_DIR_ID = -1
		internal const val STARTING_DIR_ID = 1

		private fun getAccessTypeFromNative(accessType: Int): AccessType? {
			return when (accessType) {
				ACCESS_RESOURCES.nativeValue -> ACCESS_RESOURCES
				ACCESS_FILESYSTEM.nativeValue -> ACCESS_FILESYSTEM
				else -> null
			}
		}
	}

	private enum class AccessType(val nativeValue: Int) {
		ACCESS_RESOURCES(0), ACCESS_FILESYSTEM(2)
	}

	internal interface DirectoryAccess {
		fun dirOpen(path: String): Int
		fun dirNext(dirId: Int): String
		fun dirClose(dirId: Int)
		fun dirIsDir(dirId: Int): Boolean
		fun dirExists(path: String): Boolean
		fun fileExists(path: String): Boolean
		fun hasDirId(dirId: Int): Boolean
		fun isCurrentHidden(dirId: Int): Boolean
		fun getDriveCount() : Int
		fun getDrive(drive: Int): String
		fun makeDir(dir: String): Boolean
		fun getSpaceLeft(): Long
		fun rename(from: String, to: String): Boolean
		fun remove(filename: String): Boolean
	}

	private val assetsDirAccess = AssetsDirectoryAccess(context)
	private val fileSystemDirAccess = FilesystemDirectoryAccess(context)

	private fun hasDirId(accessType: AccessType, dirId: Int): Boolean {
		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.hasDirId(dirId)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.hasDirId(dirId)
		}
	}

	fun dirOpen(nativeAccessType: Int, path: String?): Int {
		val accessType = getAccessTypeFromNative(nativeAccessType)
		if (path == null || accessType == null) {
			return INVALID_DIR_ID
		}

		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.dirOpen(path)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.dirOpen(path)
		}
	}

	fun dirNext(nativeAccessType: Int, dirId: Int): String {
		val accessType = getAccessTypeFromNative(nativeAccessType)
		if (accessType == null || !hasDirId(accessType, dirId)) {
			Log.w(TAG, "dirNext: Invalid dir id: $dirId")
			return ""
		}

		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.dirNext(dirId)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.dirNext(dirId)
		}
	}

	fun dirClose(nativeAccessType: Int, dirId: Int) {
		val accessType = getAccessTypeFromNative(nativeAccessType)
		if (accessType == null || !hasDirId(accessType, dirId)) {
			Log.w(TAG, "dirClose: Invalid dir id: $dirId")
			return
		}

		when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.dirClose(dirId)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.dirClose(dirId)
		}
	}

	fun dirIsDir(nativeAccessType: Int, dirId: Int): Boolean {
		val accessType = getAccessTypeFromNative(nativeAccessType)
		if (accessType == null || !hasDirId(accessType, dirId)) {
			Log.w(TAG, "dirIsDir: Invalid dir id: $dirId")
			return false
		}

		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.dirIsDir(dirId)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.dirIsDir(dirId)
		}
	}

	fun isCurrentHidden(nativeAccessType: Int, dirId: Int): Boolean {
		val accessType = getAccessTypeFromNative(nativeAccessType)
		if (accessType == null || !hasDirId(accessType, dirId)) {
			return false
		}

		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.isCurrentHidden(dirId)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.isCurrentHidden(dirId)
		}
	}

	fun dirExists(nativeAccessType: Int, path: String?): Boolean {
		val accessType = getAccessTypeFromNative(nativeAccessType)
		if (path == null || accessType == null) {
			return false
		}

		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.dirExists(path)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.dirExists(path)
		}
	}

	fun fileExists(nativeAccessType: Int, path: String?): Boolean {
		val accessType = getAccessTypeFromNative(nativeAccessType)
		if (path == null || accessType == null) {
			return false
		}

		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.fileExists(path)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.fileExists(path)
		}
	}

	fun getDriveCount(nativeAccessType: Int): Int {
		val accessType = getAccessTypeFromNative(nativeAccessType) ?: return 0
		return when(accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.getDriveCount()
			ACCESS_FILESYSTEM -> fileSystemDirAccess.getDriveCount()
		}
	}

	fun getDrive(nativeAccessType: Int, drive: Int): String {
		val accessType = getAccessTypeFromNative(nativeAccessType) ?: return ""
		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.getDrive(drive)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.getDrive(drive)
		}
	}

	fun makeDir(nativeAccessType: Int, dir: String): Boolean {
		val accessType = getAccessTypeFromNative(nativeAccessType) ?: return false
		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.makeDir(dir)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.makeDir(dir)
		}
	}

	fun getSpaceLeft(nativeAccessType: Int): Long {
		val accessType = getAccessTypeFromNative(nativeAccessType) ?: return 0L
		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.getSpaceLeft()
			ACCESS_FILESYSTEM -> fileSystemDirAccess.getSpaceLeft()
		}
	}

	fun rename(nativeAccessType: Int, from: String, to: String): Boolean {
		val accessType = getAccessTypeFromNative(nativeAccessType) ?: return false
		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.rename(from, to)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.rename(from, to)
		}
	}

	fun remove(nativeAccessType: Int, filename: String): Boolean {
		val accessType = getAccessTypeFromNative(nativeAccessType) ?: return false
		return when (accessType) {
			ACCESS_RESOURCES -> assetsDirAccess.remove(filename)
			ACCESS_FILESYSTEM -> fileSystemDirAccess.remove(filename)
		}
	}

}
