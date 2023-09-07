package edu.duke.raft;

import java.util.Timer;

public class LeaderMode extends RaftMode {

    private final int TIMER_ID = 5;
    private Timer myTimer = new Timer();
    private int[] followerNextIndex;

    public void go() {
        synchronized (mLock) {

            System.out.println("S" +
                    mID +
                    "." +
                    mConfig.getCurrentTerm() +
                    ": switched to leader mode.");

            followerNextIndex = new int[mConfig.getNumServers() + 1];
            for (int i = 1; i <= mConfig.getNumServers(); i++) {
                followerNextIndex[i] = mLog.getLastIndex();
            }

            if (!RaftResponses.clearAppendResponses(mConfig.getCurrentTerm())) {
                RaftServerImpl.setMode(new FollowerMode());
                return;
            }
            sendHeartbeat();
        }
    }

    private void sendHeartbeat() {
        int term = mConfig.getCurrentTerm();
        int[] responses = null;
        myTimer.cancel();
        for (int i = 1; i <= mConfig.getNumServers(); i++) {
            if (i != mID) {
                int diff = mLog.getLastIndex() - followerNextIndex[i];
                Entry[] entries = new Entry[diff];
                for (int j = 0; j < diff; j++) {
                    entries[j] = mLog.getEntry(followerNextIndex[i] + j + 1);
                }

                if (mLog.getEntry(followerNextIndex[i]) != null) {
                    RaftResponses.clearAppendResponses(mConfig.getCurrentTerm());
                    remoteAppendEntries(i, term, mID, followerNextIndex[i], mLog.getEntry(followerNextIndex[i]).term, entries, mCommitIndex);
                } else {
                    RaftResponses.clearAppendResponses(mConfig.getCurrentTerm());
                    remoteAppendEntries(i, term, mID, followerNextIndex[i], mConfig.getCurrentTerm(), entries, mCommitIndex);
                }
                if ((responses = RaftResponses.getAppendResponses(mConfig.getCurrentTerm())) != null) {
                    if (responses[i] != 0) {
                        if (responses[i] <= mConfig.getCurrentTerm()) {
                            if (followerNextIndex[i] != -1) {
                                followerNextIndex[i] -= 1;
                            }
                        } else {
                            mConfig.setCurrentTerm(responses[i], 0);
                            myTimer.cancel();
                            RaftServerImpl.setMode(new FollowerMode());
                            return;
                        }
                    } else {
                        followerNextIndex[i] = mLog.getLastIndex();
                    }
                }
            }
        }

        if (responses != null) {
            for (int i = 0; i < responses.length; i++) {
                if (responses[i] > term) {
                    mConfig.setCurrentTerm(responses[i], 0);
                    myTimer.cancel();
                    RaftServerImpl.setMode(new FollowerMode());
                    return;
                }
            }
        }


        myTimer = scheduleTimer(HEARTBEAT_INTERVAL, TIMER_ID);
    }

    // @param candidate’s term
    // @param candidate requesting vote
    // @param index of candidate’s last log entry
    // @param term of candidate’s last log entry
    // @return 0, if server votes for candidate; otherwise, server's
    // current term
    public int requestVote(int candidateTerm,
                           int candidateID,
                           int lastLogIndex,
                           int lastLogTerm) {
        synchronized (mLock) {
            int term = mConfig.getCurrentTerm();
            int vote = term;

            if (candidateTerm > term) {
                vote = candidateTerm;

                if (mLog.getLastTerm() < lastLogTerm || mLog.getLastIndex() <= lastLogIndex) {
                    mConfig.setCurrentTerm(candidateTerm, candidateID);
                    vote = 0;
                } else {
                    mConfig.setCurrentTerm(candidateTerm, 0);
                }

                myTimer.cancel();
                RaftServerImpl.setMode(new FollowerMode());
            }

            return vote;
        }
    }


    // @param leader’s term
    // @param current leader
    // @param index of log entry before entries to append
    // @param term of log entry before entries to append
    // @param entries to append (in order of 0 to append.length-1)
    // @param index of highest committed entry
    // @return 0, if server appended entries; otherwise, server's
    // current term
    public int appendEntries(int leaderTerm,
                             int leaderID,
                             int prevLogIndex,
                             int prevLogTerm,
                             Entry[] entries,
                             int leaderCommit) {
        synchronized (mLock) {
            int term = mConfig.getCurrentTerm();
            int result = term;
            boolean changeMode = false;

            if (leaderTerm > mConfig.getCurrentTerm()) {
                result = leaderTerm;

                if (entries == null || entries.length == 0) {
                    if (prevLogIndex == mLog.getLastIndex() && prevLogTerm == mLog.getLastTerm()) {
                        result = 0;
                    }
                    changeMode = true;
                }

                if (mLog.insert(entries, prevLogIndex, prevLogTerm) != -1) {
                    result = 0;
                    changeMode = true;
                }

                if (changeMode) {
                    myTimer.cancel();
                    mConfig.setCurrentTerm(leaderTerm, 0);
                    RaftServerImpl.setMode(new FollowerMode());
                }

            }

            return result;
        }
    }

    // @param id of the timer that timed out
    public void handleTimeout(int timerID) {
        synchronized (mLock) {
            sendHeartbeat();
        }
    }
}
